/*
 * scsi.c
 *
 * The scsi functions
 *
 * Copyright (C) 2001 Greg Kroah-Hartman <greg@kroah.com>
 *
 *	This program is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU General Public License as published by the
 *	Free Software Foundation version 2 of the License.
 * 
 *	This program is distributed in the hope that it will be useful, but
 *	WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	General Public License for more details.
 * 
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stddef.h>	/* for NULL */
#include <stdlib.h>	/* for getenv() */
#include <errno.h>
#include "hotplug.h"

static int scsi_add (void)
{
#ifdef WAIT_MODULE_DIR
	wait_dir_ready("/lib/modules/2.6.12.6-VENUS/kernel", 100);
	usleep(300000);
#endif
	load_module("sd_mod");
	return 0;
}


static int scsi_remove (void)
{
	/* right now we don't do anything here :) */
	return 0;
}


static struct subsystem scsi_subsystem[] = {
	{ ADD_STRING, scsi_add },
	{ REMOVE_STRING, scsi_remove },
	{ NULL, NULL }
};


int scsi_handler (void)
{
	char * action;
	
	action = getenv ("ACTION");
	dbg ("action = %s", action);
	if (action == NULL) {
		dbg ("missing ACTION environment variable, aborting.");
		return 1;
	}

	return call_subsystem (action, scsi_subsystem);
}


