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

static char IsoFile[MAXPATHLEN] = "";
static s64 cdOpenCaseTime = 0;

long CALLBACK CDR__getStatus(struct CdrStat *stat) {
	if (cdOpenCaseTime < 0 || cdOpenCaseTime > (s64)time(NULL))
		stat->Status = 0x10;
	else
		stat->Status = 0;

	return 0;
}

static unsigned char buf[256];
unsigned char stdpar[10] = { 0x00, 0x41, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char mousepar[8] = { 0x00, 0x12, 0x5a, 0xff, 0xff, 0xff, 0xff };
unsigned char analogpar[9] = { 0x00, 0xff, 0x5a, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static int bufcount, bufc;

unsigned char _PADstartPoll(PadDataS *pad) {
    bufc = 0;

    switch (pad->controllerType) {
        case PSE_PAD_TYPE_MOUSE:
            mousepar[3] = pad->buttonStatus & 0xff;
            mousepar[4] = pad->buttonStatus >> 8;
            mousepar[5] = pad->moveX;
            mousepar[6] = pad->moveY;

            memcpy(buf, mousepar, 7);
            bufcount = 6;
            break;
        case PSE_PAD_TYPE_NEGCON: // npc101/npc104(slph00001/slph00069)
            analogpar[1] = 0x23;
            analogpar[3] = pad->buttonStatus & 0xff;
            analogpar[4] = pad->buttonStatus >> 8;
            analogpar[5] = pad->rightJoyX;
            analogpar[6] = pad->rightJoyY;
            analogpar[7] = pad->leftJoyX;
            analogpar[8] = pad->leftJoyY;

            memcpy(buf, analogpar, 9);
            bufcount = 8;
            break;
        case PSE_PAD_TYPE_ANALOGPAD: // scph1150
            analogpar[1] = 0x73;
            analogpar[3] = pad->buttonStatus & 0xff;
            analogpar[4] = pad->buttonStatus >> 8;
            analogpar[5] = pad->rightJoyX;
            analogpar[6] = pad->rightJoyY;
            analogpar[7] = pad->leftJoyX;
            analogpar[8] = pad->leftJoyY;

            memcpy(buf, analogpar, 9);
            bufcount = 8;
            break;
        case PSE_PAD_TYPE_ANALOGJOY: // scph1110
            analogpar[1] = 0x53;
            analogpar[3] = pad->buttonStatus & 0xff;
            analogpar[4] = pad->buttonStatus >> 8;
            analogpar[5] = pad->rightJoyX;
            analogpar[6] = pad->rightJoyY;
            analogpar[7] = pad->leftJoyX;
            analogpar[8] = pad->leftJoyY;

            memcpy(buf, analogpar, 9);
            bufcount = 8;
            break;
        case PSE_PAD_TYPE_STANDARD:
        default:
            stdpar[3] = pad->buttonStatus & 0xff;
            stdpar[4] = pad->buttonStatus >> 8;

            memcpy(buf, stdpar, 5);
            bufcount = 4;
    }

    return buf[bufc++];
}

unsigned char _PADpoll(unsigned char value) {
    if (bufc > bufcount) return 0;
    return buf[bufc++];
}

unsigned char CALLBACK PAD1_startPoll(int pad) {
    PadDataS padd;

    PAD1_readPort1(&padd);

    return _PADstartPoll(&padd);
}

unsigned char CALLBACK PAD1_poll(unsigned char value) {
    return _PADpoll(value);
}

unsigned char CALLBACK PAD2_startPoll(int pad) {
	PadDataS padd;

	PAD2_readPort2(&padd);
    
	return _PADstartPoll(&padd);
}

unsigned char CALLBACK PAD2_poll(unsigned char value) {
  return 0xFF; //_PADpoll(value);
}

long  PadFlags = 0;

long PAD1_init(long flags) {
	PadFlags |= flags;
	
	return 0;
}

long PAD2_init(long flags) {
	PadFlags |= flags;
	
	return 0;
}

u16 ReadZodKeys()
{
	u16 pad_status = 0xffff;
//	u32 keys = gp2x_joystick_read();

/* TODO  if(	keys & GP2X_VOL_DOWN ) // L2
	{
		pad_status &= ~(1<<8);
	}
	if (keys & GP2X_L)
	{
		pad_status &= ~(1<<10); // L ?
	}
	
	if( keys & GP2X_VOL_UP ) // R2
	{
		pad_status &= ~(1<<9);
	}
	if (keys & GP2X_R)
	{
		pad_status &= ~(1<<11); // R ?
	}
  
	if (keys & GP2X_UP)
	{
		pad_status &= ~(1<<4); 
	}
	if (keys & GP2X_DOWN)
	{
		pad_status &= ~(1<<6);
	}
	if (keys & GP2X_LEFT)
	{
		pad_status &= ~(1<<7);
	}
	if (keys & GP2X_RIGHT)
	{
		pad_status &= ~(1<<5);
	}
	if (keys & GP2X_START)
	{
		pad_status &= ~(1<<3);
	}
	if (keys & GP2X_SELECT)
	{			
		pad_status &= ~(1);
	}
		
	if (keys & GP2X_X)
	{
		pad_status &= ~(1<<14);
	}
	if (keys & GP2X_B)
	{
		pad_status &= ~(1<<13);
	}
	if (keys & GP2X_A)
	{
		pad_status &= ~(1<<15);
	}
	if (keys & GP2X_Y)
	{
		pad_status &= ~(1<<12);
	}*/
	return pad_status;
}

long PAD1_readPort1(PadDataS* pad) {
	u16 pad_status = 0xffff;
	pad_status = ReadZodKeys();
	pad->buttonStatus = pad_status;
	pad->controllerType = 4; // standard	
  	return 0;
}

long PAD2_readPort1(PadDataS* pad) {
	u16 pad_status = 0xffff;
	pad_status = 0xffff;
	pad->buttonStatus = pad_status;
	pad->controllerType = 4; // standard
 	return 0;
}

long PAD1_readPort2(PadDataS* a) {
	return -1;
}

long PAD2_readPort2(PadDataS* a) {
	return -1;
}

void CALLBACK clearDynarec(void) {
	psxCpu->Reset();
}

int LoadPlugins() {
	int ret;

  cdrIsoInit();

	ret = CDR_init();
	if (ret < 0) { SysMessage (_("Error initializing CD-ROM plugin: %d"), ret); return -1; }
	ret = GPU_init();
	if (ret < 0) { SysMessage (_("Error initializing GPU plugin: %d"), ret); return -1; }
	ret = SPU_init();
	if (ret < 0) { SysMessage (_("Error initializing SPU plugin: %d"), ret); return -1; }
	ret = PAD1_init(1);
	if (ret < 0) { SysMessage (_("Error initializing Controller 1 plugin: %d"), ret); return -1; }
	ret = PAD2_init(2);
	if (ret < 0) { SysMessage (_("Error initializing Controller 2 plugin: %d"), ret); return -1; }

	ret = CDR_open();
	if (ret < 0) { SysMessage (_("Error open CD-ROM plugin: %d"), ret); return -1; }
	ret = SPU_open();
	if (ret < 0) { SysMessage (_("Error open SPU plugin: %d"), ret); return -1; }

	SysPrintf(_("Plugins loaded.\n"));
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

void SetIsoFile(const char *filename) {
	if (filename == NULL) {
		IsoFile[0] = '\0';
		return;
	}
	strncpy(IsoFile, filename, MAXPATHLEN);
}

const char *GetIsoFile(void) {
	return IsoFile;
}

boolean UsingIso(void) {
	return (IsoFile[0] != '\0');
}

void SetCdOpenCaseTime(s64 time) {
	cdOpenCaseTime = time;
}
