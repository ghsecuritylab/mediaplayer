/* 
   Unix SMB/CIFS implementation.
   oplock processing
   Copyright (C) Andrew Tridgell 1992-1998
   Copyright (C) Jeremy Allison 1998 - 2001
   Copyright (C) Volker Lendecke 2005
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define DBGC_CLASS DBGC_LOCKING
#include "includes.h"
#include "smbd/globals.h"

/****************************************************************************
 Get the number of current exclusive oplocks.
****************************************************************************/

int32 get_number_of_exclusive_open_oplocks(void)
{
  return exclusive_oplocks_open;
}

/*
 * helper function used by the kernel oplock backends to post the break message
 */
void break_kernel_oplock(struct messaging_context *msg_ctx, files_struct *fsp)
{
	uint8_t msg[MSG_SMB_KERNEL_BREAK_SIZE];

	/* Put the kernel break info into the message. */
	push_file_id_24((char *)msg, &fsp->file_id);
	SIVAL(msg,24,fsp->fh->gen_id);

	/* Don't need to be root here as we're only ever
	   sending to ourselves. */

	messaging_send_buf(msg_ctx, procid_self(),
			   MSG_SMB_KERNEL_BREAK,
			   msg, MSG_SMB_KERNEL_BREAK_SIZE);
}

/****************************************************************************
 Attempt to set an oplock on a file. Always succeeds if kernel oplocks are
 disabled (just sets flags). Returns True if oplock set.
****************************************************************************/

bool set_file_oplock(files_struct *fsp, int oplock_type)
{
	if ((fsp->oplock_type == LEVEL_II_OPLOCK)
	    && koplocks && !(koplocks->flags & KOPLOCKS_LEVEL2_SUPPORTED)) {
		DEBUG(10, ("Refusing level2 oplock, kernel oplocks don't "
			   "support them\n"));
		return false;
	}
	if ((fsp->oplock_type != NO_OPLOCK) &&
	    (fsp->oplock_type != FAKE_LEVEL_II_OPLOCK) &&
	    koplocks &&
	    !koplocks->ops->set_oplock(koplocks, fsp, oplock_type)) {
		return False;
	}

	fsp->oplock_type = oplock_type;
	fsp->sent_oplock_break = NO_BREAK_SENT;
	if (oplock_type == LEVEL_II_OPLOCK) {
		level_II_oplocks_open++;
	} else if (EXCLUSIVE_OPLOCK_TYPE(fsp->oplock_type)) {
		exclusive_oplocks_open++;
	}

	DEBUG(5,("set_file_oplock: granted oplock on file %s, %s/%lu, "
		    "tv_sec = %x, tv_usec = %x\n",
		 fsp->fsp_name, file_id_string_tos(&fsp->file_id),
		 fsp->fh->gen_id, (int)fsp->open_time.tv_sec,
		 (int)fsp->open_time.tv_usec ));

	return True;
}

/****************************************************************************
 Attempt to release an oplock on a file. Decrements oplock count.
****************************************************************************/

void release_file_oplock(files_struct *fsp)
{
	if ((fsp->oplock_type != NO_OPLOCK) &&
	    (fsp->oplock_type != FAKE_LEVEL_II_OPLOCK) &&
	    koplocks) {
		koplocks->ops->release_oplock(koplocks, fsp, NO_OPLOCK);
	}

	if (fsp->oplock_type == LEVEL_II_OPLOCK) {
		level_II_oplocks_open--;
	} else if (EXCLUSIVE_OPLOCK_TYPE(fsp->oplock_type)) {
		exclusive_oplocks_open--;
	}

	SMB_ASSERT(exclusive_oplocks_open>=0);
	SMB_ASSERT(level_II_oplocks_open>=0);

	if (EXCLUSIVE_OPLOCK_TYPE(fsp->oplock_type)) {
		/* This doesn't matter for close. */
		fsp->oplock_type = FAKE_LEVEL_II_OPLOCK;
	} else {
		fsp->oplock_type = NO_OPLOCK;
	}
	fsp->sent_oplock_break = NO_BREAK_SENT;

	flush_write_cache(fsp, OPLOCK_RELEASE_FLUSH);

	TALLOC_FREE(fsp->oplock_timeout);
}

/****************************************************************************
 Attempt to downgrade an oplock on a file. Doesn't decrement oplock count.
****************************************************************************/

static void downgrade_file_oplock(files_struct *fsp)
{
	if (!EXCLUSIVE_OPLOCK_TYPE(fsp->oplock_type)) {
		DEBUG(0, ("trying to downgrade an already-downgraded oplock!\n"));
		return;
	}

	if (koplocks) {
		koplocks->ops->release_oplock(koplocks, fsp, LEVEL_II_OPLOCK);
	}
	fsp->oplock_type = LEVEL_II_OPLOCK;
	exclusive_oplocks_open--;
	level_II_oplocks_open++;
	fsp->sent_oplock_break = NO_BREAK_SENT;
}

/****************************************************************************
 Remove a file oplock. Copes with level II and exclusive.
 Locks then unlocks the share mode lock. Client can decide to go directly
 to none even if a "break-to-level II" was sent.
****************************************************************************/

bool remove_oplock(files_struct *fsp)
{
	bool ret;
	struct share_mode_lock *lck;

	/* Remove the oplock flag from the sharemode. */
	lck = get_share_mode_lock(talloc_tos(), fsp->file_id, NULL, NULL,
				  NULL);
	if (lck == NULL) {
		DEBUG(0,("remove_oplock: failed to lock share entry for "
			 "file %s\n", fsp->fsp_name ));
		return False;
	}
	ret = remove_share_oplock(lck, fsp);
	if (!ret) {
		DEBUG(0,("remove_oplock: failed to remove share oplock for "
			 "file %s fnum %d, %s\n",
			 fsp->fsp_name, fsp->fnum, file_id_string_tos(&fsp->file_id)));
	}
	release_file_oplock(fsp);
	TALLOC_FREE(lck);
	return ret;
}

/*
 * Deal with a reply when a break-to-level II was sent.
 */
bool downgrade_oplock(files_struct *fsp)
{
	bool ret;
	struct share_mode_lock *lck;

	lck = get_share_mode_lock(talloc_tos(), fsp->file_id, NULL, NULL,
				  NULL);
	if (lck == NULL) {
		DEBUG(0,("downgrade_oplock: failed to lock share entry for "
			 "file %s\n", fsp->fsp_name ));
		return False;
	}
	ret = downgrade_share_oplock(lck, fsp);
	if (!ret) {
		DEBUG(0,("downgrade_oplock: failed to downgrade share oplock "
			 "for file %s fnum %d, file_id %s\n",
			 fsp->fsp_name, fsp->fnum, file_id_string_tos(&fsp->file_id)));
	}

	downgrade_file_oplock(fsp);
	TALLOC_FREE(lck);
	return ret;
}

/*
 * Some kernel oplock implementations handle the notification themselves.
 */
bool should_notify_deferred_opens()
{
	return !(koplocks &&
		(koplocks->flags & KOPLOCKS_DEFERRED_OPEN_NOTIFICATION));
}

/****************************************************************************
 Set up an oplock break message.
****************************************************************************/

static char *new_break_smb_message(TALLOC_CTX *mem_ctx,
				   files_struct *fsp, uint8 cmd)
{
	char *result = TALLOC_ARRAY(mem_ctx, char, smb_size + 8*2 + 0);

	if (result == NULL) {
		DEBUG(0, ("talloc failed\n"));
		return NULL;
	}

	memset(result,'\0',smb_size);
	srv_set_message(result,8,0,true);
	SCVAL(result,smb_com,SMBlockingX);
	SSVAL(result,smb_tid,fsp->conn->cnum);
	SSVAL(result,smb_pid,0xFFFF);
	SSVAL(result,smb_uid,0);
	SSVAL(result,smb_mid,0xFFFF);
	SCVAL(result,smb_vwv0,0xFF);
	SSVAL(result,smb_vwv2,fsp->fnum);
	SCVAL(result,smb_vwv3,LOCKING_ANDX_OPLOCK_RELEASE);
	SCVAL(result,smb_vwv3+1,cmd);
	return result;
}

/****************************************************************************
 Function to do the waiting before sending a local break.
****************************************************************************/

static void wait_before_sending_break(void)
{
	long wait_time = (long)lp_oplock_break_wait_time();

	if (wait_time) {
		smb_msleep(wait_time);
	}
}

/****************************************************************************
 Ensure that we have a valid oplock.
****************************************************************************/

static files_struct *initial_break_processing(struct file_id id, unsigned long file_id)
{
	files_struct *fsp = NULL;

	if( DEBUGLVL( 3 ) ) {
		dbgtext( "initial_break_processing: called for %s/%u\n",
			 file_id_string_tos(&id), (int)file_id);
		dbgtext( "Current oplocks_open (exclusive = %d, levelII = %d)\n",
			exclusive_oplocks_open, level_II_oplocks_open );
	}

	/*
	 * We need to search the file open table for the
	 * entry containing this dev and inode, and ensure
	 * we have an oplock on it.
	 */

	fsp = file_find_dif(id, file_id);

	if(fsp == NULL) {
		/* The file could have been closed in the meantime - return success. */
		if( DEBUGLVL( 3 ) ) {
			dbgtext( "initial_break_processing: cannot find open file with " );
			dbgtext( "file_id %s gen_id = %lu", file_id_string_tos(&id), file_id);
			dbgtext( "allowing break to succeed.\n" );
		}
		return NULL;
	}

	/* Ensure we have an oplock on the file */

	/*
	 * There is a potential race condition in that an oplock could
	 * have been broken due to another udp request, and yet there are
	 * still oplock break messages being sent in the udp message
	 * queue for this file. So return true if we don't have an oplock,
	 * as we may have just freed it.
	 */

	if(fsp->oplock_type == NO_OPLOCK) {
		if( DEBUGLVL( 3 ) ) {
			dbgtext( "initial_break_processing: file %s ", fsp->fsp_name );
			dbgtext( "(file_id = %s gen_id = %lu) has no oplock.\n",
				 file_id_string_tos(&id), fsp->fh->gen_id );
			dbgtext( "Allowing break to succeed regardless.\n" );
		}
		return NULL;
	}

	return fsp;
}

static void oplock_timeout_handler(struct event_context *ctx,
				   struct timed_event *te,
				   struct timeval now,
				   void *private_data)
{
	files_struct *fsp = (files_struct *)private_data;

	/* Remove the timed event handler. */
	TALLOC_FREE(fsp->oplock_timeout);
	DEBUG(0, ("Oplock break failed for file %s -- replying anyway\n", fsp->fsp_name));
	global_client_failed_oplock_break = True;
	remove_oplock(fsp);
	reply_to_oplock_break_requests(fsp);
}

/*******************************************************************
 Add a timeout handler waiting for the client reply.
*******************************************************************/

static void add_oplock_timeout_handler(files_struct *fsp)
{
	/*
	 * If kernel oplocks already notifies smbds when an oplock break times
	 * out, just return.
	 */
	if (koplocks &&
	    (koplocks->flags & KOPLOCKS_TIMEOUT_NOTIFICATION)) {
		return;
	}

	if (fsp->oplock_timeout != NULL) {
		DEBUG(0, ("Logic problem -- have an oplock event hanging "
			  "around\n"));
	}

	fsp->oplock_timeout =
		event_add_timed(smbd_event_context(), NULL,
				timeval_current_ofs(OPLOCK_BREAK_TIMEOUT, 0),
				oplock_timeout_handler, fsp);

	if (fsp->oplock_timeout == NULL) {
		DEBUG(0, ("Could not add oplock timeout handler\n"));
	}
}

void break_level2_to_none_async(files_struct *fsp)
{
	char *break_msg;
	bool sign_state;

	if (fsp->oplock_type == NO_OPLOCK) {
		/* We already got a "break to none" message and we've handled
		 * it.  just ignore. */
		DEBUG(3, ("process_oplock_async_level2_break_message: already "
			  "broken to none, ignoring.\n"));
		return;
	}

	if (fsp->oplock_type == FAKE_LEVEL_II_OPLOCK) {
		/* Don't tell the client, just downgrade. */
		DEBUG(3, ("process_oplock_async_level2_break_message: "
			  "downgrading fake level 2 oplock.\n"));
		remove_oplock(fsp);
		return;
	}

	/* Ensure we're really at level2 state. */
	SMB_ASSERT(fsp->oplock_type == LEVEL_II_OPLOCK);

	DEBUG(10,("process_oplock_async_level2_break_message: sending break "
		  "to none message for fid %d, file %s\n", fsp->fnum,
		  fsp->fsp_name));

	/* Now send a break to none message to our client. */
	break_msg = new_break_smb_message(NULL, fsp, OPLOCKLEVEL_NONE);
	if (break_msg == NULL) {
		exit_server("Could not talloc break_msg\n");
	}

	/* Save the server smb signing state. */
	sign_state = srv_oplock_set_signing(False);

	show_msg(break_msg);
	if (!srv_send_smb(smbd_server_fd(),
			break_msg,
			IS_CONN_ENCRYPTED(fsp->conn),
			NULL)) {
		exit_server_cleanly("oplock_break: srv_send_smb failed.");
	}

	/* Restore the sign state to what it was. */
	srv_oplock_set_signing(sign_state);

	TALLOC_FREE(break_msg);

	/* Async level2 request, don't send a reply, just remove the oplock. */
	remove_oplock(fsp);

}

/*******************************************************************
 This handles the case of a write triggering a break to none
 message on a level2 oplock.
 When we get this message we may be in any of three states :
 NO_OPLOCK, LEVEL_II, FAKE_LEVEL2. We only send a message to
 the client for LEVEL2.
*******************************************************************/

void process_oplock_async_level2_break_message(struct messaging_context *msg_ctx,
						      void *private_data,
						      uint32_t msg_type,
						      struct server_id src,
						      DATA_BLOB *data)
{
	struct share_mode_entry msg;
	files_struct *fsp;

	if (data->data == NULL) {
		DEBUG(0, ("Got NULL buffer\n"));
		return;
	}

	if (data->length != MSG_SMB_SHARE_MODE_ENTRY_SIZE) {
		DEBUG(0, ("Got invalid msg len %d\n", (int)data->length));
		return;
	}

	/* De-linearize incoming message. */
	message_to_share_mode_entry(&msg, (char *)data->data);

	DEBUG(10, ("Got oplock async level 2 break message from pid %s: "
		   "%s/%lu\n", procid_str(debug_ctx(), &src),
		   file_id_string_tos(&msg.id), msg.share_file_id));

	fsp = initial_break_processing(msg.id, msg.share_file_id);

	if (fsp == NULL) {
		/* We hit a race here. Break messages are sent, and before we
		 * get to process this message, we have closed the file. 
		 * No need to reply as this is an async message. */
		DEBUG(3, ("process_oplock_async_level2_break_message: Did not find fsp, ignoring\n"));
		return;
	}

	break_level2_to_none_async(fsp);
}

/*******************************************************************
 This handles the generic oplock break message from another smbd.
*******************************************************************/

static void process_oplock_break_message(struct messaging_context *msg_ctx,
					 void *private_data,
					 uint32_t msg_type,
					 struct server_id src,
					 DATA_BLOB *data)
{
	struct share_mode_entry msg;
	files_struct *fsp;
	char *break_msg;
	bool break_to_level2 = False;
	bool sign_state;

	if (data->data == NULL) {
		DEBUG(0, ("Got NULL buffer\n"));
		return;
	}

	if (data->length != MSG_SMB_SHARE_MODE_ENTRY_SIZE) {
		DEBUG(0, ("Got invalid msg len %d\n", (int)data->length));
		return;
	}

	/* De-linearize incoming message. */
	message_to_share_mode_entry(&msg, (char *)data->data);

	DEBUG(10, ("Got oplock break message from pid %s: %s/%lu\n",
		   procid_str(debug_ctx(), &src), file_id_string_tos(&msg.id),
		   msg.share_file_id));

	fsp = initial_break_processing(msg.id, msg.share_file_id);

	if (fsp == NULL) {
		/* a We hit race here. Break messages are sent, and before we
		 * get to process this message, we have closed the file. Reply
		 * with 'ok, oplock broken' */
		DEBUG(3, ("Did not find fsp\n"));

		/* We just send the same message back. */
		messaging_send_buf(msg_ctx, src, MSG_SMB_BREAK_RESPONSE,
				   (uint8 *)data->data,
				   MSG_SMB_SHARE_MODE_ENTRY_SIZE);
		return;
	}

	if (fsp->sent_oplock_break != NO_BREAK_SENT) {
		/* Remember we have to inform the requesting PID when the
		 * client replies */
		msg.pid = src;
		ADD_TO_ARRAY(NULL, struct share_mode_entry, msg,
			     &fsp->pending_break_messages,
			     &fsp->num_pending_break_messages);
		return;
	}

	if (EXCLUSIVE_OPLOCK_TYPE(msg.op_type) &&
	    !EXCLUSIVE_OPLOCK_TYPE(fsp->oplock_type)) {
		DEBUG(3, ("Already downgraded oplock on %s: %s\n",
			  file_id_string_tos(&fsp->file_id),
			  fsp->fsp_name));
		/* We just send the same message back. */
		messaging_send_buf(msg_ctx, src, MSG_SMB_BREAK_RESPONSE,
				   (uint8 *)data->data,
				   MSG_SMB_SHARE_MODE_ENTRY_SIZE);
		return;
	}

	if ((global_client_caps & CAP_LEVEL_II_OPLOCKS) && 
	    !(msg.op_type & FORCE_OPLOCK_BREAK_TO_NONE) &&
	    !(koplocks && !(koplocks->flags & KOPLOCKS_LEVEL2_SUPPORTED)) &&
	    lp_level2_oplocks(SNUM(fsp->conn))) {
		break_to_level2 = True;
	}

	break_msg = new_break_smb_message(NULL, fsp, break_to_level2 ?
					  OPLOCKLEVEL_II : OPLOCKLEVEL_NONE);
	if (break_msg == NULL) {
		exit_server("Could not talloc break_msg\n");
	}

	/* Need to wait before sending a break message if we sent ourselves this message. */
	if (procid_is_me(&src)) {
		wait_before_sending_break();
	}

	/* Save the server smb signing state. */
	sign_state = srv_oplock_set_signing(False);

	show_msg(break_msg);
	if (!srv_send_smb(smbd_server_fd(),
			break_msg,
			IS_CONN_ENCRYPTED(fsp->conn),
			NULL)) {
		exit_server_cleanly("oplock_break: srv_send_smb failed.");
	}

	/* Restore the sign state to what it was. */
	srv_oplock_set_signing(sign_state);

	TALLOC_FREE(break_msg);

	fsp->sent_oplock_break = break_to_level2 ? LEVEL_II_BREAK_SENT:BREAK_TO_NONE_SENT;

	msg.pid = src;
	ADD_TO_ARRAY(NULL, struct share_mode_entry, msg,
		     &fsp->pending_break_messages,
		     &fsp->num_pending_break_messages);

	add_oplock_timeout_handler(fsp);
}

/*******************************************************************
 This handles the kernel oplock break message.
*******************************************************************/

static void process_kernel_oplock_break(struct messaging_context *msg_ctx,
					void *private_data,
					uint32_t msg_type,
					struct server_id src,
					DATA_BLOB *data)
{
	struct file_id id;
	unsigned long file_id;
	files_struct *fsp;
	char *break_msg;
	bool sign_state;

	if (data->data == NULL) {
		DEBUG(0, ("Got NULL buffer\n"));
		return;
	}

	if (data->length != MSG_SMB_KERNEL_BREAK_SIZE) {
		DEBUG(0, ("Got invalid msg len %d\n", (int)data->length));
		return;
	}

	/* Pull the data from the message. */
	pull_file_id_24((char *)data->data, &id);
	file_id = (unsigned long)IVAL(data->data, 24);

	DEBUG(10, ("Got kernel oplock break message from pid %s: %s/%u\n",
		   procid_str(debug_ctx(), &src), file_id_string_tos(&id),
		   (unsigned int)file_id));

	fsp = initial_break_processing(id, file_id);

	if (fsp == NULL) {
		DEBUG(3, ("Got a kernel oplock break message for a file "
			  "I don't know about\n"));
		return;
	}

	if (fsp->sent_oplock_break != NO_BREAK_SENT) {
		/* This is ok, kernel oplocks come in completely async */
		DEBUG(3, ("Got a kernel oplock request while waiting for a "
			  "break reply\n"));
		return;
	}

	break_msg = new_break_smb_message(NULL, fsp, OPLOCKLEVEL_NONE);
	if (break_msg == NULL) {
		exit_server("Could not talloc break_msg\n");
	}

	/* Save the server smb signing state. */
	sign_state = srv_oplock_set_signing(False);

	show_msg(break_msg);
	if (!srv_send_smb(smbd_server_fd(),
			break_msg,
			IS_CONN_ENCRYPTED(fsp->conn),
			NULL)) {
		exit_server_cleanly("oplock_break: srv_send_smb failed.");
	}

	/* Restore the sign state to what it was. */
	srv_oplock_set_signing(sign_state);

	TALLOC_FREE(break_msg);

	fsp->sent_oplock_break = BREAK_TO_NONE_SENT;

	add_oplock_timeout_handler(fsp);
}

void reply_to_oplock_break_requests(files_struct *fsp)
{
	int i;

	/*
	 * If kernel oplocks already notifies smbds when oplocks are
	 * broken/removed, just return.
	 */
	if (koplocks &&
	    (koplocks->flags & KOPLOCKS_OPLOCK_BROKEN_NOTIFICATION)) {
		return;
	}

	for (i=0; i<fsp->num_pending_break_messages; i++) {
		struct share_mode_entry *e = &fsp->pending_break_messages[i];
		char msg[MSG_SMB_SHARE_MODE_ENTRY_SIZE];

		share_mode_entry_to_message(msg, e);

		messaging_send_buf(smbd_messaging_context(), e->pid,
				   MSG_SMB_BREAK_RESPONSE,
				   (uint8 *)msg,
				   MSG_SMB_SHARE_MODE_ENTRY_SIZE);
	}

	SAFE_FREE(fsp->pending_break_messages);
	fsp->num_pending_break_messages = 0;
	if (fsp->oplock_timeout != NULL) {
		/* Remove the timed event handler. */
		TALLOC_FREE(fsp->oplock_timeout);
		fsp->oplock_timeout = NULL;
	}
	return;
}

static void process_oplock_break_response(struct messaging_context *msg_ctx,
					  void *private_data,
					  uint32_t msg_type,
					  struct server_id src,
					  DATA_BLOB *data)
{
	struct share_mode_entry msg;

	if (data->data == NULL) {
		DEBUG(0, ("Got NULL buffer\n"));
		return;
	}

	if (data->length != MSG_SMB_SHARE_MODE_ENTRY_SIZE) {
		DEBUG(0, ("Got invalid msg len %u\n",
			  (unsigned int)data->length));
		return;
	}

	/* De-linearize incoming message. */
	message_to_share_mode_entry(&msg, (char *)data->data);

	DEBUG(10, ("Got oplock break response from pid %s: %s/%lu mid %u\n",
		   procid_str(debug_ctx(), &src), file_id_string_tos(&msg.id),
		   msg.share_file_id, (unsigned int)msg.op_mid));

	/* Here's the hack from open.c, store the mid in the 'port' field */
	schedule_deferred_open_smb_message(msg.op_mid);
}

static void process_open_retry_message(struct messaging_context *msg_ctx,
				       void *private_data,
				       uint32_t msg_type,
				       struct server_id src,
				       DATA_BLOB *data)
{
	struct share_mode_entry msg;
	
	if (data->data == NULL) {
		DEBUG(0, ("Got NULL buffer\n"));
		return;
	}

	if (data->length != MSG_SMB_SHARE_MODE_ENTRY_SIZE) {
		DEBUG(0, ("Got invalid msg len %d\n", (int)data->length));
		return;
	}

	/* De-linearize incoming message. */
	message_to_share_mode_entry(&msg, (char *)data->data);

	DEBUG(10, ("Got open retry msg from pid %s: %s mid %u\n",
		   procid_str(debug_ctx(), &src), file_id_string_tos(&msg.id),
		   (unsigned int)msg.op_mid));

	schedule_deferred_open_smb_message(msg.op_mid);
}

/****************************************************************************
 This function is called on any file modification or lock request. If a file
 is level 2 oplocked then it must tell all other level 2 holders to break to
 none.
****************************************************************************/

static void contend_level2_oplocks_begin_default(files_struct *fsp,
					      enum level2_contention_type type)
{
	int i;
	struct share_mode_lock *lck;

	/*
	 * If this file is level II oplocked then we need
	 * to grab the shared memory lock and inform all
	 * other files with a level II lock that they need
	 * to flush their read caches. We keep the lock over
	 * the shared memory area whilst doing this.
	 */

	if (!LEVEL_II_OPLOCK_TYPE(fsp->oplock_type))
		return;

	lck = get_share_mode_lock(talloc_tos(), fsp->file_id, NULL, NULL,
				  NULL);
	if (lck == NULL) {
		DEBUG(0,("release_level_2_oplocks_on_change: failed to lock "
			 "share mode entry for file %s.\n", fsp->fsp_name ));
		return;
	}

	DEBUG(10,("release_level_2_oplocks_on_change: num_share_modes = %d\n", 
		  lck->num_share_modes ));

	for(i = 0; i < lck->num_share_modes; i++) {
		struct share_mode_entry *share_entry = &lck->share_modes[i];
		char msg[MSG_SMB_SHARE_MODE_ENTRY_SIZE];

		if (!is_valid_share_mode_entry(share_entry)) {
			continue;
		}

		/*
		 * As there could have been multiple writes waiting at the
		 * lock_share_entry gate we may not be the first to
		 * enter. Hence the state of the op_types in the share mode
		 * entries may be partly NO_OPLOCK and partly LEVEL_II or FAKE_LEVEL_II
		 * oplock. It will do no harm to re-send break messages to
		 * those smbd's that are still waiting their turn to remove
		 * their LEVEL_II state, and also no harm to ignore existing
		 * NO_OPLOCK states. JRA.
		 */

		DEBUG(10,("release_level_2_oplocks_on_change: "
			  "share_entry[%i]->op_type == %d\n",
			  i, share_entry->op_type ));

		if (share_entry->op_type == NO_OPLOCK) {
			continue;
		}

		/* Paranoia .... */
		if (EXCLUSIVE_OPLOCK_TYPE(share_entry->op_type)) {
			DEBUG(0,("release_level_2_oplocks_on_change: PANIC. "
				 "share mode entry %d is an exlusive "
				 "oplock !\n", i ));
			TALLOC_FREE(lck);
			abort();
		}

		share_mode_entry_to_message(msg, share_entry);

		/*
		 * Deal with a race condition when breaking level2
 		 * oplocks. Don't send all the messages and release
 		 * the lock, this allows someone else to come in and
 		 * get a level2 lock before any of the messages are
 		 * processed, and thus miss getting a break message.
 		 * Ensure at least one entry (the one we're breaking)
 		 * is processed immediately under the lock and becomes
 		 * set as NO_OPLOCK to stop any waiter getting a level2.
 		 * Bugid #5980.
 		 */

		if (procid_is_me(&share_entry->pid)) {
			wait_before_sending_break();
			break_level2_to_none_async(fsp);
		} else {
			messaging_send_buf(smbd_messaging_context(),
					share_entry->pid,
					MSG_SMB_ASYNC_LEVEL2_BREAK,
					(uint8 *)msg,
					MSG_SMB_SHARE_MODE_ENTRY_SIZE);
		}
	}

	/* We let the message receivers handle removing the oplock state
	   in the share mode lock db. */

	TALLOC_FREE(lck);
}

void contend_level2_oplocks_begin(files_struct *fsp,
				  enum level2_contention_type type)
{
	if (koplocks && koplocks->ops->contend_level2_oplocks_begin) {
		koplocks->ops->contend_level2_oplocks_begin(fsp, type);
		return;
	}

	contend_level2_oplocks_begin_default(fsp, type);
}

void contend_level2_oplocks_end(files_struct *fsp,
				enum level2_contention_type type)
{
	/* Only kernel oplocks implement this so far */
	if (koplocks && koplocks->ops->contend_level2_oplocks_end) {
		koplocks->ops->contend_level2_oplocks_end(fsp, type);
	}
}

/****************************************************************************
 Linearize a share mode entry struct to an internal oplock break message.
****************************************************************************/

void share_mode_entry_to_message(char *msg, const struct share_mode_entry *e)
{
	SIVAL(msg,0,(uint32)e->pid.pid);
	SSVAL(msg,4,e->op_mid);
	SSVAL(msg,6,e->op_type);
	SIVAL(msg,8,e->access_mask);
	SIVAL(msg,12,e->share_access);
	SIVAL(msg,16,e->private_options);
	SIVAL(msg,20,(uint32)e->time.tv_sec);
	SIVAL(msg,24,(uint32)e->time.tv_usec);
	push_file_id_24(msg+28, &e->id);
	SIVAL(msg,52,e->share_file_id);
	SIVAL(msg,56,e->uid);
	SSVAL(msg,60,e->flags);
#ifdef CLUSTER_SUPPORT
	SIVAL(msg,62,e->pid.vnn);
#endif
}

/****************************************************************************
 De-linearize an internal oplock break message to a share mode entry struct.
****************************************************************************/

void message_to_share_mode_entry(struct share_mode_entry *e, char *msg)
{
	e->pid.pid = (pid_t)IVAL(msg,0);
	e->op_mid = SVAL(msg,4);
	e->op_type = SVAL(msg,6);
	e->access_mask = IVAL(msg,8);
	e->share_access = IVAL(msg,12);
	e->private_options = IVAL(msg,16);
	e->time.tv_sec = (time_t)IVAL(msg,20);
	e->time.tv_usec = (int)IVAL(msg,24);
	pull_file_id_24(msg+28, &e->id);
	e->share_file_id = (unsigned long)IVAL(msg,52);
	e->uid = (uint32)IVAL(msg,56);
	e->flags = (uint16)SVAL(msg,60);
#ifdef CLUSTER_SUPPORT
	e->pid.vnn = IVAL(msg,62);
#endif
}

/****************************************************************************
 Setup oplocks for this process.
****************************************************************************/

bool init_oplocks(struct messaging_context *msg_ctx)
{
	DEBUG(3,("init_oplocks: initializing messages.\n"));

	messaging_register(msg_ctx, NULL, MSG_SMB_BREAK_REQUEST,
			   process_oplock_break_message);
	messaging_register(msg_ctx, NULL, MSG_SMB_ASYNC_LEVEL2_BREAK,
			   process_oplock_async_level2_break_message);
	messaging_register(msg_ctx, NULL, MSG_SMB_BREAK_RESPONSE,
			   process_oplock_break_response);
	messaging_register(msg_ctx, NULL, MSG_SMB_KERNEL_BREAK,
			   process_kernel_oplock_break);
	messaging_register(msg_ctx, NULL, MSG_SMB_OPEN_RETRY,
			   process_open_retry_message);

	if (lp_kernel_oplocks()) {
#if HAVE_KERNEL_OPLOCKS_IRIX
		koplocks = irix_init_kernel_oplocks(talloc_autofree_context());
#elif HAVE_KERNEL_OPLOCKS_LINUX
		koplocks = linux_init_kernel_oplocks(talloc_autofree_context());
#elif HAVE_ONEFS
		koplocks = onefs_init_kernel_oplocks(talloc_autofree_context());
#endif
	}

	return True;
}
