/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA
 */

#include "psxcommon.h"
#include "sio.h"
#include "ppf.h"

int SysInit() {
#ifdef EMU_LOG
#ifndef LOG_STDOUT
	emuLog = fopen("emuLog.txt","wb");
#else
	emuLog = stdout;
#endif
	setvbuf(emuLog, NULL, _IONBF, 0);
#endif

	if (psxInit() == -1) {
		printf("PSX emulator couldn't be initialized.\n");
		return -1;
	}

	LoadMcds(Config.Mcd1, Config.Mcd2);	/* TODO Do we need to have this here, or in the calling main() function?? */

	if (Config.Debug) {
		StartDebugger();
	}

	return 0;
}

static void dummy_lace()
{
}

void SysReset() {
	// rearmed hack: psxReset() runs some code when real BIOS is used,
	// but we usually do reset from menu while GPU is not open yet,
	// so we need to prevent updateLace() call..
	void (*real_lace)() = GPUupdateLace;
	GPU_updateLace = dummy_lace;

	psxReset();

	// hmh core forgets this
	CDR_stop();

	GPU_updateLace = real_lace;
}

void SysClose() {
	FreePPFCache();
	psxShutdown();

	ReleasePlugins();

	StopDebugger();

	if (emuLog != NULL) fclose(emuLog);
}

void SysPrintf(const char *fmt, ...) {
#if 0
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	if (Config.PsxOut) {
		static char linestart = 1;
		int l = strlen(msg);

		printf(linestart ? " * %s" : "%s", msg);

		if (l > 0 && msg[l - 1] == '\n') {
			linestart = 1;
		} else {
			linestart = 0;
		}
	}

#ifdef EMU_LOG
#ifndef LOG_STDOUT
	fprintf(emuLog, "%s", msg);
#endif
#endif
#endif
}

void SysMessage(const char *fmt, ...) {
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);
	qDebug(msg);
#if 0
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	if (Config.PsxOut) {
		static char linestart = 1;
		int l = strlen(msg);

		printf(linestart ? " * %s" : "%s", msg);

		if (l > 0 && msg[l - 1] == '\n') {
			linestart = 1;
		} else {
			linestart = 0;
		}
	}

#ifdef EMU_LOG
#ifndef LOG_STDOUT
	fprintf(emuLog, "%s", msg);
#endif
#endif
#endif
}
