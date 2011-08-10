#include "nesapuchannel.h"

NesApuChannel::NesApuChannel(int channelNo) :
	channelNo(channelNo) {
}

void NesApuChannel::reset() {
	enable = false;
	userEnable = true;

	masterVolume = 0;
	volume = 0;

	lengthCounterEnable = false;
	lengthCounter = 0;

	envelopeVolume = 0;
	envelopeDecayRate = 0;
	envelopeDecayDisable = false;
	envelopeDecayLoopEnable = false;
	envelopeDecayCounter = 0;
	envelopeReset = false;

	progTimerMax = 0;
	progTimerCount = 0;

	dutyMode = 0;
	sampleCondition = false;
	sampleValue = 0;
}

/*
4000h - APU Volume/Decay Channel 1 (Rectangle)
4004h - APU Volume/Decay Channel 2 (Rectangle)
400Ch - APU Volume/Decay Channel 4 (Noise)
  0-3    Volume / Envelope decay rate
		  When Bit4=1: Volume (0=Silent/None..F=Loud/Max)
		  When Bit4=0: Envelope decay rate, NTSC=240Hz/(N+1), PAL=192Hz/(N+1)
  4      Envelope decay disable (0=Envelope/Decay, 1=Fixed Volume)
  5      Length counter clock disable / Envelope decay looping enable
		  When Bit4=1: length counter clock disable
		  When Bit4=0: envelope decay looping enable
		   0: Disable Looping, stay at 0 on end of decay [ \_____ ]
		   1: Enable Looping, restart decay at F         [ \\\\\\ ]
		   (Does this still affect Length counter clock disable ?)
  6-7    Duty cycle type (unused on noise channel)
			0  [--______________] 12.5%   Whereas,
			1  [----____________] 25.0%   [_] = LOW  (zero) (0)
			2  [--------________] 50.0%   [-] = HIGH (volume/decay) (0..F)
			3  [------------____] 75.0%   Noise randomly outputs LOW or HIGH
The Duty Cycle counter is reset when the length counter of the same channel is written to (via $4003/$4007).

Initial Decay Volume:
 Only a write out to $4003/$4007/$400F will reset the current envelope decay
 counter to a known state (to $F, the maximum volume level) for the
 appropriate channel's envelope decay hardware. Otherwise, the envelope decay
 counter is always counting down (by 1) at the frequency currently contained
 in the volume / envelope decay rate bits (even when envelope decays are
 disabled by setting bit 4), except when the envelope decay counter contains a
 value of 0, and envelope decay looping (bit 5) is disabled (0).

*/
void NesApuChannel::setVolumeDecay(quint8 data) {
	volume = data & 0x0F;
	envelopeDecayRate = (data & 0x0F) + 1;
	envelopeDecayDisable = data & 0x10;
	lengthCounterEnable = !(data & 0x20);
	envelopeDecayLoopEnable = data & 0x20;
	dutyMode = (data >> 6) & 0x03;
	updateMasterVolume();
	updateSampleCondition();
}

void NesApuChannel::updateMasterVolume()
{ masterVolume = envelopeDecayDisable ? volume : envelopeVolume; }

/*
4002h - APU Frequency Channel 1 (Rectangle)
4006h - APU Frequency Channel 2 (Rectangle)
400Ah - APU Frequency Channel 3 (Triangle)
  0-7     Lower 8 bits of wavelength (upper 3 bits in Register 3)

F = 1.79MHz/(N+1)/16 for Rectangle channels
F = 1.79MHz/(N+1)/32 for Triangle channel


400Eh - APU Frequency Channel 4 (Noise)  0-3     Noise frequency, F=1.79MHz/2/(N+1)
		   Value 0..F corresponds to following 11bit clock cycle value:
		   N=002,004,008,010,020,030,040,050,065,07F,0BE,0FE,17D,1FC,3F9,7F2
  4-6     Unused
  7       Random number type generation (0=32767 bits, 1=93 bits)

The random number generator consists of a 15bit shift register.
The MSB (Bit14) is output/inverted (1=Low/Zero, 0=High/Decay/Volume).
At the specified frequency, Bit14 is XORed with Bit13 (32767-bit mode) or with Bit8 (93-bit mode),
the register is then shifted to the left, with the result of the XOR operation shifted-in to Bit0.

 On 2A03 reset, this shift register is loaded with a value of 1.
 Not sure if it is reset when switching from 32767-bit mode to 93-bit mode?
 If it isn't reset then 93-bit mode will act unstable:
 produce different 93-bit patterns, or even a 31-bit pattern,
 depending on old shift register content.
 */
void NesApuChannel::setFrequency(quint8 data) {
	progTimerMax = (progTimerMax & ~0xFF) | data;
	updateSampleCondition();
}

/*
4003h - APU Length Channel 1 (Rectangle)
4007h - APU Length Channel 2 (Rectangle)
400Bh - APU Length Channel 3 (Triangle)
400Fh - APU Length Channel 4 (Noise)
 Writing to the length registers restarts the length (obviously),
 and also restarts the duty cycle (channel 1,2 only),
 and restarts the decay volume (channel 1,2,4 only).
  2-0   Upper 3 bits of wavelength (unused on noise channel)
  7-3   Length counter load register (5bit value, see below)

The above 5bit value is translated to the actual 7bit counter value as such:
  Bit3=0 and Bit7=0 (Dividers matched for use with PAL/50Hz)
	Bit6-4  (0..7 = 05h,0Ah,14h,28h,50h,1Eh,07h,0Dh)
  Bit3=0 and Bit7=1 (Dividers matched for use with NTSC/60Hz)
	Bit6-4  (0..7 = 06h,0Ch,18h,30h,60h,24h,08h,10h)
  Bit3=1 (General Fixed Dividers)
	Bit7-4  (0..F = 7Fh,01h..0Fh)

The 7bit counter value is decremented once per frame (PAL=48Hz, or NTSC=60Hz)
the counter and sound output are stopped when reaching a value of zero.
The counter can be paused (and restarted at current location) by Length Counter Clock Disabled bit in Register 0.
 */
void NesApuChannel::setLength(quint8 data) {
	progTimerMax = (progTimerMax & 0xFF) | ((data & 0x07) << 8);
	if (enable)
		lengthCounter = lengthMaxLUT[data >> 3];
	envelopeReset = true;
	updateSampleCondition();
}

int NesApuChannel::lengthMaxLUT[32] = {
	0x0A, 0xFE, 0x14, 0x02, 0x28, 0x04, 0x50, 0x06, 0xA0, 0x08, 0x3C, 0x0A, 0x0E, 0x0C, 0x1A, 0x0E,
	0x0C, 0x10, 0x18, 0x12, 0x30, 0x14, 0x60, 0x16, 0xC0, 0x18, 0x48, 0x1A, 0x10, 0x1C, 0x20, 0x1E
};

void NesApuChannel::setEnabled(bool on){
	on &= userEnable;
	if (enable == on)
		return;
	enable = on;
	if (!on)
		lengthCounter = 0;
	updateSampleCondition();
	updateSampleValue();
}

void NesApuChannel::setUserEnabled(bool on) {
	userEnable = on;
	setEnabled(enable);
}

void NesApuChannel::updateSampleCondition()
{ sampleCondition = (lengthStatus() && progTimerMax > 7); }

void NesApuChannel::clockLengthCounter() {
	if (lengthCounterEnable && lengthCounter > 0){
		lengthCounter--;
		if (!lengthCounter) {
			updateSampleCondition();
			updateSampleValue();
		}
	}
}

void NesApuChannel::clockEnvelopeDecay() {
	if (envelopeReset) {
		envelopeReset = false;
		envelopeDecayCounter = envelopeDecayRate;
		envelopeVolume = 0x0F;
	} else if (--envelopeDecayCounter <= 0) {
		envelopeDecayCounter = envelopeDecayRate;
		if (envelopeVolume > 0)
			envelopeVolume--;
		else
			envelopeVolume = (envelopeDecayLoopEnable ? 0x0F : 0x00);
	}
	updateMasterVolume();
	updateSampleCondition();
	updateSampleValue();
}
