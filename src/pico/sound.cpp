// This is part of Pico Library

// (c) Copyright 2004 Dave, All rights reserved.
// (c) Copyright 2006,2007 notaz, All rights reserved.
// Free for non-commercial use.

// For commercial use, separate licencing terms must be obtained.

#include <string.h>
#include "ym2612.h"
#include "sn76496.h"

#include "machine.h"
#include "pico.h"
#include "cd_pcm.h"
#include <QtEndian>

bool picoSoundEnabled = true;

static const int PicoSoundOutBufferSize = 8192;
static u32 picoSoundOutBuffer[PicoSoundOutBufferSize];
static int picoSoundOutHead = 0;
static int picoSoundOutTail = 0;

static int picoSoundMixBuffer[2*44100/50]__attribute__((aligned(4))); // TODO needed ??

static unsigned short dac_info[312]; // pppppppp ppppllll, p - pos in buff, l - length to write for this sample

// for Pico
int PsndRate=0;
int PsndLen=0; // number of mono samples, multiply by 2 for stereo
int PsndLen_exc_add=0; // this is for non-integer sample counts per line, eg. 22050/60
int PsndLen_exc_cnt=0;

// sn76496
extern int *sn76496_regs;

static void picoSoundFinishMixing(int offset, int count)
{
	int *src = picoSoundMixBuffer + offset;
	u32 *dst = picoSoundOutBuffer;
	int l, r;
	for (; count > 0; count--) {
		l = qBound(S32_MIN, *src++, S32_MAX);
		r = qBound(S32_MIN, *src++, S32_MAX);
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
		u32 sample = l | (r << 16);
#else
		u32 sample = (l << 16) | r;
#endif
		dst[picoSoundOutHead] = sample;
		picoSoundOutHead++;
		picoSoundOutHead &= PicoSoundOutBufferSize-1;
	}
}

int PicoMachine::fillAudioBuffer(char *stream, int streamSize) {
	u32 *src = picoSoundOutBuffer;
	u32 *dst = (u32 *)stream;
	int available = (picoSoundOutHead-picoSoundOutTail+PicoSoundOutBufferSize) & (PicoSoundOutBufferSize-1);
	int length = qMin(available, streamSize>>2);

	if ((picoSoundOutTail+length) >= PicoSoundOutBufferSize) {
		u32 partialLength = PicoSoundOutBufferSize - picoSoundOutTail;
		memcpy(dst, src+picoSoundOutTail, partialLength<<2);
		// picoSoundOutTail = 0;
		u32 remainderLength = length - partialLength;
		memcpy(dst+partialLength, src, remainderLength<<2);
		picoSoundOutTail = remainderLength;
	} else {
		memcpy(dst, src+picoSoundOutTail, length<<2);
		picoSoundOutTail += length;
	}
	return length<<2;
}

void PicoMachine::setAudioEnabled(bool on) {
	picoSoundEnabled = on;
}

static void dac_recalculate(void)
{
	int i, dac_cnt, pos, len, lines = Pico.m.pal ? 312 : 262, mid = Pico.m.pal ? 68 : 93;

	if(PsndLen <= lines) {
		// shrinking algo
		dac_cnt = -PsndLen;
		len=1; pos=0;
		dac_info[225] = 1;

		for(i=226; i != 225; i++) {
			if (i >= lines) i = 0;
			len = 0;
			if(dac_cnt < 0) {
				len=1;
				pos++;
				dac_cnt += lines;
			}
			dac_cnt -= PsndLen;
			dac_info[i] = (pos<<4)|len;
		}
	} else {
		// stretching
		dac_cnt = PsndLen;
		pos=0;
		for(i = 225; i != 224; i++) {
			if (i >= lines) i = 0;
			len=0;
			while(dac_cnt >= 0) {
				dac_cnt -= lines;
				len++;
			}
			if (i == mid) // midpoint
				while(pos+len < PsndLen/2) {
					dac_cnt -= lines;
					len++;
				}
			dac_cnt += PsndLen;
			dac_info[i] = (pos<<4)|len;
			pos+=len;
		}
		// last sample
		for(len = 0, i = pos; i < PsndLen; i++) len++;
		if (PsndLen_exc_add) len++;
		dac_info[224] = (pos<<4)|len;
	}
	//for(i=len=0; i < lines; i++) {
	//  printf("%03i : %03i : %i\n", i, dac_info[i]>>4, dac_info[i]&0xf);
	//  len+=dac_info[i]&0xf;
	//}
	//printf("rate is %i, len %f\n", PsndRate, (qreal)PsndRate/(Pico.m.pal ? 50.0 : 60.0));
	//printf("len total: %i, last pos: %i\n", len, pos);
	//exit(8);
}


void PsndReset(void)
{
	void *ym2612_regs;

	// also clear the internal registers+addr line
	ym2612_regs = YM2612GetRegs();
	memset(ym2612_regs, 0, 0x200+4);
	z80startCycle = z80stopCycle = 0;

	PsndRerate(0);
}


// to be called after changing sound rate or chips
void PsndRerate(int preserve_state)
{
	void *state = NULL;
	int target_fps = Pico.m.pal ? 50 : 60;

	// not all rates are supported in MCD mode due to mp3 decoder limitations
	if (PicoMCD & 1) {
		if (PsndRate != 11025 && PsndRate != 22050 && PsndRate != 44100) PsndRate = 22050;
		PicoOpt |= 8; // force stereo
	}

	if (preserve_state) {
		state = malloc(0x200);
		if (state == NULL) return;
		memcpy(state, YM2612GetRegs(), 0x200);
		if ((PicoMCD & 1) && Pico_mcd->m.audio_track)
			Pico_mcd->m.audio_offset = 0;// TODO mp3_get_offset();
	}
	YM2612Init(Pico.m.pal ? OSC_PAL/7 : OSC_NTSC/7, PsndRate);
	if (preserve_state) {
		// feed it back it's own registers, just like after loading state
		memcpy(YM2612GetRegs(), state, 0x200);
		YM2612PicoStateLoad();
		// TODO if ((PicoMCD & 1) && Pico_mcd->m.audio_track)
		//  mp3_start_play((FILE*)Pico_mcd->TOC.Tracks[Pico_mcd->m.audio_track].F, Pico_mcd->m.audio_offset);
	}

	if (preserve_state) memcpy(state, sn76496_regs, 28*4); // remember old state
	SN76496Init(Pico.m.pal ? OSC_PAL/15 : OSC_NTSC/15, PsndRate);
	if (preserve_state) memcpy(sn76496_regs, state, 28*4); // restore old state

	if (state)
		free(state);

	// calculate PsndLen
	PsndLen=PsndRate / target_fps;
	PsndLen_exc_add=((PsndRate - PsndLen*target_fps)<<16) / target_fps;
	PsndLen_exc_cnt=0;

	// recalculate dac info
	dac_recalculate();

	if (PicoMCD & 1)
		pcm_set_rate(PsndRate);

	// clear all buffers
	memset32(picoSoundMixBuffer, 0, sizeof(picoSoundMixBuffer)/4);
	if (picoSoundEnabled)
		PsndClear();
}

// This is called once per raster (aka line), but not necessarily for every line
void Psnd_timers_and_dac(int raster)
{
	int do_dac = picoSoundEnabled && (PicoOpt&1) && *ym2612_dacen;

	// Our raster lasts 63.61323/64.102564 microseconds (NTSC/PAL)
	YM2612PicoTick(1);

	if (!do_dac)
		return;

	int pos = dac_info[raster];
	int len = pos & 0xf;
	if (!len)
		return;

	pos >>= 4;

	int *d = picoSoundMixBuffer + pos*2;
	int dout = *ym2612_dacout;
	// some manual loop unrolling here :)
	d[0] = dout;
	if (len > 1) {
		d[2] = dout;
		if (len > 2)
			d[4] = dout;
	}
}

void PsndClear()
{
	int len = PsndLen;
	if (PsndLen_exc_add)
		len++;
//	memset32((int *) picoSoundOutBuffer, 0, len); // assume picoSoundOutBuffer to be aligned
}

int PsndRender(int offset, int length)
{
	int *buf32 = picoSoundMixBuffer+offset;
	// emulating CD && PCM option enabled && PCM chip on && have enabled channels
	int do_pcm = (PicoMCD&1) && (PicoOpt&0x400) && (Pico_mcd->pcm.control & 0x80) && Pico_mcd->pcm.enabled;
	offset <<= 1;

	if (offset == 0) { // should happen once per frame
		// compensate for float part of PsndLen
		PsndLen_exc_cnt += PsndLen_exc_add;
		if (PsndLen_exc_cnt >= 0x10000) {
			PsndLen_exc_cnt -= 0x10000;
			length++;
		}
	}

	// PSG
	if (PicoOpt & 2)
		SN76496Update(buf32, length);

	// Add in the stereo FM buffer
	if (PicoOpt & 1)
		YM2612UpdateOne(buf32, length);

	// CD: PCM sound
	if (do_pcm)
		pcm_update(buf32, length);

	// CD: CDDA audio
	// CD mode, cdda enabled, not data track, CDC is reading
	// TODO if ((PicoMCD & 1) && (PicoOpt & 0x800) && !(Pico_mcd->s68k_regs[0x36] & 1) && (Pico_mcd->scd.Status_CDC & 1))
	//  mp3_update(buf32, length, stereo);

	// convert + limit to normal 16bit output

	picoSoundFinishMixing(offset, length);
	memset32(buf32, 0, length<<1);

	return length;
}

struct DrZ80 drZ80;

static unsigned int DrZ80_rebasePC(unsigned short a)
{
	drZ80.Z80PC_BASE = (unsigned int) Pico.zram;
	return drZ80.Z80PC_BASE + a;
}

static unsigned int DrZ80_rebaseSP(unsigned short a)
{
	drZ80.Z80SP_BASE = (unsigned int) Pico.zram;
	return drZ80.Z80SP_BASE + a;
}

static void DrZ80_irq_callback()
{
	drZ80.Z80_IRQ = 0; // lower irq when accepted
}

static unsigned char z80_in(unsigned short p)
{
	elprintf(EL_ANOMALY, "Z80 port %04x read", p);
	return 0xff;
}

static void z80_out(unsigned short p,unsigned char d)
{
	elprintf(EL_ANOMALY, "Z80 port %04x write %02x", p, d);
}

// z80 functionality wrappers
void z80_init()
{
	memset(&drZ80, 0, sizeof(struct DrZ80));
	drZ80.z80_rebasePC=DrZ80_rebasePC;
	drZ80.z80_rebaseSP=DrZ80_rebaseSP;
	drZ80.z80_read8   =z80_read;
	drZ80.z80_read16  =z80_read16;
	drZ80.z80_write8  =z80_write;
	drZ80.z80_write16 =z80_write16;
	drZ80.z80_in      =z80_in;
	drZ80.z80_out     =z80_out;
	drZ80.z80_irq_callback=DrZ80_irq_callback;
}

void z80_reset(void)
{
	memset(&drZ80, 0, 0x54);
	drZ80.Z80F  = (1<<2);  // set ZFlag
	drZ80.Z80F2 = (1<<2);  // set ZFlag
	drZ80.Z80IX = 0xFFFF << 16;
	drZ80.Z80IY = 0xFFFF << 16;
	drZ80.Z80IM = 0; // 1?
	drZ80.Z80PC = drZ80.z80_rebasePC(0);
	drZ80.Z80SP = drZ80.z80_rebaseSP(0x2000); // 0xf000 ?

	Pico.m.z80_fakeval = 0; // for faking when Z80 is disabled
}


void z80_pack(unsigned char *data)
{
	*(int *)data = 0x015A7244; // "DrZ" v1
	drZ80.Z80PC = drZ80.z80_rebasePC(drZ80.Z80PC-drZ80.Z80PC_BASE);
	drZ80.Z80SP = drZ80.z80_rebaseSP(drZ80.Z80SP-drZ80.Z80SP_BASE);
	memcpy(data+4, &drZ80, 0x54);
}

void z80_unpack(unsigned char *data)
{
	if (*(int *)data == 0x015A7244) { // "DrZ" v1 save?
		memcpy(&drZ80, data+4, 0x54);
		// update bases
		drZ80.Z80PC = drZ80.z80_rebasePC(drZ80.Z80PC-drZ80.Z80PC_BASE);
		drZ80.Z80SP = drZ80.z80_rebaseSP(drZ80.Z80SP-drZ80.Z80SP_BASE);
	} else {
		z80_reset();
		drZ80.Z80IM = 1;
		z80_int(); // try to goto int handler, maybe we won't execute trash there?
	}
}

void z80_debug(char *dstr)
{
	sprintf(dstr, "Z80 state: PC: %04x SP: %04x\n", drZ80.Z80PC-drZ80.Z80PC_BASE, drZ80.Z80SP-drZ80.Z80SP_BASE);
}
