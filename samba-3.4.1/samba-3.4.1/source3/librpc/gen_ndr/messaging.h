/* header auto-generated by pidl */

#include <stdint.h>

#ifndef _HEADER_messaging
#define _HEADER_messaging

#define MSG_TYPE_MASK	( 0xFFFF )
enum messaging_type
#ifndef USE_UINT_ENUMS
 {
	MSG_DEBUG=0x0001,
	MSG_PING=0x0002,
	MSG_PONG=0x0003,
	MSG_PROFILE=0x0004,
	MSG_REQ_DEBUGLEVEL=0x0005,
	MSG_DEBUGLEVEL=0x0006,
	MSG_REQ_PROFILELEVEL=0x0007,
	MSG_PROFILELEVEL=0x0008,
	MSG_REQ_POOL_USAGE=0x0009,
	MSG_POOL_USAGE=0x000A,
	MSG_REQ_DMALLOC_MARK=0x000B,
	MSG_REQ_DMALLOC_LOG_CHANGED=0x000C,
	MSG_SHUTDOWN=0x000D,
	MSG_FORCE_ELECTION=0x0101,
	MSG_WINS_NEW_ENTRY=0x0102,
	MSG_SEND_PACKET=0x0103,
	MSG_PRINTER_NOTIFY=0x2001,
	MSG_PRINTER_NOTIFY2=0x0202,
	MSG_PRINTER_DRVUPGRADE=0x0203,
	MSG_PRINTERDATA_INIT_RESET=0x0204,
	MSG_PRINTER_UPDATE=0x0205,
	MSG_PRINTER_MOD=0x0206,
	MSG_SMB_CONF_UPDATED=0x0301,
	MSG_SMB_FORCE_TDIS=0x0302,
	MSG_SMB_SAM_SYNC=0x0303,
	MSG_SMB_SAM_REPL=0x0304,
	MSG_SMB_UNLOCK=0x0305,
	MSG_SMB_BREAK_REQUEST=0x0306,
	MSG_SMB_BREAK_RESPONSE=0x0307,
	MSG_SMB_ASYNC_LEVEL2_BREAK=0x0308,
	MSG_SMB_OPEN_RETRY=0x0309,
	MSG_SMB_KERNEL_BREAK=0x030A,
	MSG_SMB_FILE_RENAME=0x030B,
	MSG_SMB_INJECT_FAULT=0x030C,
	MSG_SMB_BLOCKING_LOCK_CANCEL=0x030D,
	MSG_SMB_NOTIFY=0x030E,
	MSG_SMB_STAT_CACHE_DELETE=0x030F,
	MSG_PVFS_NOTIFY=0x0310,
	MSG_SMB_BRL_VALIDATE=0x0311,
	MSG_SMB_RELEASE_IP=0x0312,
	MSG_SMB_CLOSE_FILE=0x0313,
	MSG_WINBIND_FINISHED=0x0401,
	MSG_WINBIND_FORGET_STATE=0x0402,
	MSG_WINBIND_ONLINE=0x0403,
	MSG_WINBIND_OFFLINE=0x0404,
	MSG_WINBIND_ONLINESTATUS=0x0405,
	MSG_WINBIND_TRY_TO_GO_ONLINE=0x0406,
	MSG_WINBIND_FAILED_TO_GO_ONLINE=0x0407,
	MSG_WINBIND_VALIDATE_CACHE=0x0408,
	MSG_WINBIND_DUMP_DOMAIN_LIST=0x0409,
	MSG_DUMP_EVENT_LIST=0x0500,
	MSG_DBWRAP_TDB2_CHANGES=4001
}
#else
 { __donnot_use_enum_messaging_type=0x7FFFFFFF}
#define MSG_DEBUG ( 0x0001 )
#define MSG_PING ( 0x0002 )
#define MSG_PONG ( 0x0003 )
#define MSG_PROFILE ( 0x0004 )
#define MSG_REQ_DEBUGLEVEL ( 0x0005 )
#define MSG_DEBUGLEVEL ( 0x0006 )
#define MSG_REQ_PROFILELEVEL ( 0x0007 )
#define MSG_PROFILELEVEL ( 0x0008 )
#define MSG_REQ_POOL_USAGE ( 0x0009 )
#define MSG_POOL_USAGE ( 0x000A )
#define MSG_REQ_DMALLOC_MARK ( 0x000B )
#define MSG_REQ_DMALLOC_LOG_CHANGED ( 0x000C )
#define MSG_SHUTDOWN ( 0x000D )
#define MSG_FORCE_ELECTION ( 0x0101 )
#define MSG_WINS_NEW_ENTRY ( 0x0102 )
#define MSG_SEND_PACKET ( 0x0103 )
#define MSG_PRINTER_NOTIFY ( 0x2001 )
#define MSG_PRINTER_NOTIFY2 ( 0x0202 )
#define MSG_PRINTER_DRVUPGRADE ( 0x0203 )
#define MSG_PRINTERDATA_INIT_RESET ( 0x0204 )
#define MSG_PRINTER_UPDATE ( 0x0205 )
#define MSG_PRINTER_MOD ( 0x0206 )
#define MSG_SMB_CONF_UPDATED ( 0x0301 )
#define MSG_SMB_FORCE_TDIS ( 0x0302 )
#define MSG_SMB_SAM_SYNC ( 0x0303 )
#define MSG_SMB_SAM_REPL ( 0x0304 )
#define MSG_SMB_UNLOCK ( 0x0305 )
#define MSG_SMB_BREAK_REQUEST ( 0x0306 )
#define MSG_SMB_BREAK_RESPONSE ( 0x0307 )
#define MSG_SMB_ASYNC_LEVEL2_BREAK ( 0x0308 )
#define MSG_SMB_OPEN_RETRY ( 0x0309 )
#define MSG_SMB_KERNEL_BREAK ( 0x030A )
#define MSG_SMB_FILE_RENAME ( 0x030B )
#define MSG_SMB_INJECT_FAULT ( 0x030C )
#define MSG_SMB_BLOCKING_LOCK_CANCEL ( 0x030D )
#define MSG_SMB_NOTIFY ( 0x030E )
#define MSG_SMB_STAT_CACHE_DELETE ( 0x030F )
#define MSG_PVFS_NOTIFY ( 0x0310 )
#define MSG_SMB_BRL_VALIDATE ( 0x0311 )
#define MSG_SMB_RELEASE_IP ( 0x0312 )
#define MSG_SMB_CLOSE_FILE ( 0x0313 )
#define MSG_WINBIND_FINISHED ( 0x0401 )
#define MSG_WINBIND_FORGET_STATE ( 0x0402 )
#define MSG_WINBIND_ONLINE ( 0x0403 )
#define MSG_WINBIND_OFFLINE ( 0x0404 )
#define MSG_WINBIND_ONLINESTATUS ( 0x0405 )
#define MSG_WINBIND_TRY_TO_GO_ONLINE ( 0x0406 )
#define MSG_WINBIND_FAILED_TO_GO_ONLINE ( 0x0407 )
#define MSG_WINBIND_VALIDATE_CACHE ( 0x0408 )
#define MSG_WINBIND_DUMP_DOMAIN_LIST ( 0x0409 )
#define MSG_DUMP_EVENT_LIST ( 0x0500 )
#define MSG_DBWRAP_TDB2_CHANGES ( 4001 )
#endif
;

struct messaging_rec {
	uint32_t msg_version;
	enum messaging_type msg_type;
	struct server_id dest;
	struct server_id src;
	DATA_BLOB buf;
}/* [public] */;

struct messaging_array {
	uint32_t num_messages;
	struct messaging_rec *messages;
}/* [public] */;

struct dbwrap_tdb2_changes {
	const char *magic_string;/* [value("TDB2"),charset(DOS)] */
	uint32_t magic_version;/* [value] */
	const char *name;/* [charset(UTF8)] */
	uint32_t old_seqnum;
	uint32_t new_seqnum;
	uint32_t num_changes;
	uint32_t num_keys;
	DATA_BLOB *keys;
}/* [public] */;

#endif /* _HEADER_messaging */
