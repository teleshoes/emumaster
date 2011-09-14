/***************************************************************************
                          freeze.c  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

#include "stdafx.h"

#define _IN_FREEZE

#include "externals.h"
#include "registers.h"
#include "spu.h"
#include "regs.h"

////////////////////////////////////////////////////////////////////////
// freeze structs
////////////////////////////////////////////////////////////////////////
/*
typedef struct
{
 int            State;
 int            AttackModeExp;
 int            AttackRate;
 int            DecayRate;
 int            SustainLevel;
 int            SustainModeExp;
 int            SustainIncrease;
 int            SustainRate;
 int            ReleaseModeExp;
 int            ReleaseRate;
 int            EnvelopeVol;
 long           lVolume;
 long           lDummy1;
 long           lDummy2;
} ADSRInfoEx_orig;

typedef struct
{
 // no mutexes used anymore... don't need them to sync access
 //HANDLE            hMutex;

 int               bNew;                               // start flag

 int               iSBPos;                             // mixing stuff
 int               spos;
 int               sinc;
 int               SB[32+32];                          // Pete added another 32 dwords in 1.6 ... prevents overflow issues with gaussian/cubic interpolation (thanx xodnizel!), and can be used for even better interpolations, eh? :)
 int               sval;

 unsigned char *   pStart;                             // start ptr into sound mem
 unsigned char *   pCurr;                              // current pos in sound mem
 unsigned char *   pLoop;                              // loop ptr in sound mem

 int               bOn;                                // is channel active (sample playing?)
 int               bStop;                              // is channel stopped (sample _can_ still be playing, ADSR Release phase)
 int               bReverb;                            // can we do reverb on this channel? must have ctrl register bit, to get active
 int               iActFreq;                           // current psx pitch
 int               iUsedFreq;                          // current pc pitch
 int               iLeftVolume;                        // left volume
 int               iLeftVolRaw;                        // left psx volume value
 int               bIgnoreLoop;                        // ignore loop bit, if an external loop address is used
 int               iMute;                              // mute mode
 int               iRightVolume;                       // right volume
 int               iRightVolRaw;                       // right psx volume value
 int               iRawPitch;                          // raw pitch (0...3fff)
 int               iIrqDone;                           // debug irq done flag
 int               s_1;                                // last decoding infos
 int               s_2;
 int               bRVBActive;                         // reverb active flag
 int               iRVBOffset;                         // reverb offset
 int               iRVBRepeat;                         // reverb repeat
 int               bNoise;                             // noise active flag
 int               bFMod;                              // freq mod (0=off, 1=sound channel, 2=freq channel)
 int               iRVBNum;                            // another reverb helper
 int               iOldNoise;                          // old noise val for this channel   
 ADSRInfo          ADSR;                               // active ADSR settings
 ADSRInfoEx_orig   ADSRX;                              // next ADSR settings (will be moved to active on sample start)
} SPUCHAN_orig;

typedef struct
{
 unsigned short  spuIrq;
 uint32_t   pSpuIrq;
 uint32_t   spuAddr;
 uint32_t   dummy1;
 uint32_t   dummy2;
 uint32_t   dummy3;

 SPUCHAN_orig s_chan[MAXCHAN];   

} SPUOSSFreeze_t;

////////////////////////////////////////////////////////////////////////

void LoadStateV5(SPUFreeze_t * pF);                    // newest version

extern int lastch;

// we want to retain compatibility between versions,
// so use original channel struct
static void save_channel(SPUCHAN_orig *d, SPUCHAN *s, int ch)
{
 memset(d, 0, sizeof(*d));
 d->bNew = !!(dwNewChannel & (1<<ch));
 d->iSBPos = s->iSBPos;
 d->spos = s->spos;
 d->sinc = s->sinc;
 memcpy(d->SB, s->SB, sizeof(d->SB));
 d->pStart = s->pStart;
 d->pCurr = s->pCurr;
 d->pLoop = s->pLoop;
 d->bOn = !!(dwChannelOn & (1<<ch));
 d->bStop = s->bStop;
 d->bReverb = s->bReverb;
 d->iActFreq = s->iActFreq;
 d->iUsedFreq = s->iUsedFreq;
 d->iLeftVolume = s->iLeftVolume;
 d->bIgnoreLoop = s->bIgnoreLoop;
 d->iRightVolume = s->iRightVolume;
 d->iRawPitch = s->iRawPitch;
 d->s_1 = s->s_1;
 d->s_2 = s->s_2;
 d->bRVBActive = s->bRVBActive;
 d->iRVBOffset = s->iRVBOffset;
 d->iRVBRepeat = s->iRVBRepeat;
 d->bNoise = s->bNoise;
 d->bFMod = s->bFMod;
 d->iRVBNum = s->iRVBNum;
 d->iOldNoise = s->iOldNoise;
 d->ADSRX.State = s->ADSRX.State;
 d->ADSRX.AttackModeExp = s->ADSRX.AttackModeExp;
 d->ADSRX.AttackRate = s->ADSRX.AttackRate;
 d->ADSRX.DecayRate = s->ADSRX.DecayRate;
 d->ADSRX.SustainLevel = s->ADSRX.SustainLevel;
 d->ADSRX.SustainModeExp = s->ADSRX.SustainModeExp;
 d->ADSRX.SustainIncrease = s->ADSRX.SustainIncrease;
 d->ADSRX.SustainRate = s->ADSRX.SustainRate;
 d->ADSRX.ReleaseModeExp = s->ADSRX.ReleaseModeExp;
 d->ADSRX.ReleaseRate = s->ADSRX.ReleaseRate;
 d->ADSRX.EnvelopeVol = s->ADSRX.EnvelopeVol;
 d->ADSRX.lVolume = d->bOn; // hmh
}

static void load_channel(SPUCHAN *d, SPUCHAN_orig *s, int ch)
{
 memset(d, 0, sizeof(*d));
 if (s->bNew) dwNewChannel |= 1<<ch;
 d->iSBPos = s->iSBPos;
 d->spos = s->spos;
 d->sinc = s->sinc;
 memcpy(d->SB, s->SB, sizeof(d->SB));
 d->pStart = s->pStart;
 d->pCurr = s->pCurr;
 d->pLoop = s->pLoop;
 if (s->bOn) dwChannelOn |= 1<<ch;
 d->bStop = s->bStop;
 d->bReverb = s->bReverb;
 d->iActFreq = s->iActFreq;
 d->iUsedFreq = s->iUsedFreq;
 d->iLeftVolume = s->iLeftVolume;
 d->bIgnoreLoop = s->bIgnoreLoop;
 d->iRightVolume = s->iRightVolume;
 d->iRawPitch = s->iRawPitch;
 d->s_1 = s->s_1;
 d->s_2 = s->s_2;
 d->bRVBActive = s->bRVBActive;
 d->iRVBOffset = s->iRVBOffset;
 d->iRVBRepeat = s->iRVBRepeat;
 d->bNoise = s->bNoise;
 d->bFMod = s->bFMod;
 d->iRVBNum = s->iRVBNum;
 d->iOldNoise = s->iOldNoise;
 d->ADSRX.State = s->ADSRX.State;
 d->ADSRX.AttackModeExp = s->ADSRX.AttackModeExp;
 d->ADSRX.AttackRate = s->ADSRX.AttackRate;
 d->ADSRX.DecayRate = s->ADSRX.DecayRate;
 d->ADSRX.SustainLevel = s->ADSRX.SustainLevel;
 d->ADSRX.SustainModeExp = s->ADSRX.SustainModeExp;
 d->ADSRX.SustainIncrease = s->ADSRX.SustainIncrease;
 d->ADSRX.SustainRate = s->ADSRX.SustainRate;
 d->ADSRX.ReleaseModeExp = s->ADSRX.ReleaseModeExp;
 d->ADSRX.ReleaseRate = s->ADSRX.ReleaseRate;
 d->ADSRX.EnvelopeVol = s->ADSRX.EnvelopeVol;
}

////////////////////////////////////////////////////////////////////////
// SPUFREEZE: called by main emu on savestate load/save
////////////////////////////////////////////////////////////////////////

long SPU_freeze(uint32_t ulFreezeMode,SPUFreeze_t * pF)
{
 int i;SPUOSSFreeze_t * pFO;

 if(ulFreezeMode)                                      // info or save?
  {//--------------------------------------------------//
   memcpy(pF->cSPURam,spuMem,0x80000);                 // copy common infos
   memcpy(pF->cSPUPort,regArea,0x200);

   if(xapGlobal && XAPlay!=XAFeed)                     // some xa
    {
     pF->xaS=*xapGlobal;     
    }
   else 
   memset(&pF->xaS,0,sizeof(xa_decode_t));             // or clean xa

   pFO=(SPUOSSFreeze_t *)(pF+1);                       // store special stuff

   pFO->spuIrq=spuIrq;
   if(pSpuIrq)  pFO->pSpuIrq  = (unsigned long)pSpuIrq-(unsigned long)spuMemC;

   pFO->spuAddr=spuAddr;
   if(pFO->spuAddr==0) pFO->spuAddr=0xbaadf00d;

   for(i=0;i<MAXCHAN;i++)
    {
     save_channel(&pFO->s_chan[i],&s_chan[i],i);
     if(pFO->s_chan[i].pStart)
      pFO->s_chan[i].pStart-=(unsigned long)spuMemC;
     if(pFO->s_chan[i].pCurr)
      pFO->s_chan[i].pCurr-=(unsigned long)spuMemC;
     if(pFO->s_chan[i].pLoop)
      pFO->s_chan[i].pLoop-=(unsigned long)spuMemC;
    }

   return 1;
   //--------------------------------------------------//
  }

 memcpy(spuMem,pF->cSPURam,0x80000);                   // get ram
 memcpy(regArea,pF->cSPUPort,0x200);

 if(pF->xaS.nsamples<=4032)                            // start xa again
  SPU_playADPCMchannel(&pF->xaS);

 xapGlobal=0;
   LoadStateV5(pF);

 lastch = -1;

 // repair some globals
 for(i=0;i<=62;i+=2)
  SPU_writeRegister(H_Reverb+i,regArea[(H_Reverb+i-0xc00)>>1]);
 SPU_writeRegister(H_SPUReverbAddr,regArea[(H_SPUReverbAddr-0xc00)>>1]);
 SPU_writeRegister(H_SPUrvolL,regArea[(H_SPUrvolL-0xc00)>>1]);
 SPU_writeRegister(H_SPUrvolR,regArea[(H_SPUrvolR-0xc00)>>1]);

 SPU_writeRegister(H_SPUctrl,(unsigned short)(regArea[(H_SPUctrl-0xc00)>>1]|0x4000));
 SPU_writeRegister(H_SPUstat,regArea[(H_SPUstat-0xc00)>>1]);
 SPU_writeRegister(H_CDLeft,regArea[(H_CDLeft-0xc00)>>1]);
 SPU_writeRegister(H_CDRight,regArea[(H_CDRight-0xc00)>>1]);

 // fix to prevent new interpolations from crashing
 for(i=0;i<MAXCHAN;i++) s_chan[i].SB[28]=0;

 SetupTimer();                                         // start sound processing again

 return 1;
}

////////////////////////////////////////////////////////////////////////

void LoadStateV5(SPUFreeze_t * pF)
{
 int i;SPUOSSFreeze_t * pFO;

 pFO=(SPUOSSFreeze_t *)(pF+1);

 spuIrq = pFO->spuIrq;
 if(pFO->pSpuIrq) pSpuIrq = pFO->pSpuIrq+spuMemC; else pSpuIrq=NULL;

 if(pFO->spuAddr)
  {
   spuAddr = pFO->spuAddr;
   if (spuAddr == 0xbaadf00d) spuAddr = 0;
  }

 dwNewChannel=0;
 dwChannelOn=0;
 for(i=0;i<MAXCHAN;i++)
  {
   load_channel(&s_chan[i],&pFO->s_chan[i],i);

   s_chan[i].pStart+=(unsigned long)spuMemC;
   s_chan[i].pCurr+=(unsigned long)spuMemC;
   s_chan[i].pLoop+=(unsigned long)spuMemC;
  }
}
*/
