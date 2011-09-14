/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

/*
* Plugin library callback/access functions.
*/

#include "plugins.h"
#include "cdriso.h"
#include <time.h>

int LoadPlugins() {
	int ret;
	cdrIsoInit();
	ret = CDR_init();
	if (ret < 0) { SysMessage ("Error initializing CD-ROM plugin: %d", ret); return -1; }
	ret = GPU_init();
	if (ret < 0) { SysMessage ("Error initializing GPU plugin: %d", ret); return -1; }
	ret = SPU_init();
	if (ret < 0) { SysMessage ("Error initializing SPU plugin: %d", ret); return -1; }
	ret = CDR_open();
	if (ret < 0) { SysMessage ("Error open CD-ROM plugin: %d", ret); return -1; }
	ret = SPU_open();
	if (ret < 0) { SysMessage ("Error open SPU plugin: %d", ret); return -1; }
	return 0;
}

void ReleasePlugins() {
  s32 ret;
	NetOpened = FALSE;

	ret = CDR_close();
	if (ret < 0) { SysMessage("Error Closing CDR Plugin"); }
	ret = SPU_close();
	if (ret < 0) { SysMessage("Error Closing SPU Plugin"); }

  CDR_shutdown();
  SPU_shutdown();
}
