#include "nesapurectanglechannel.h"

NesApuRectangleChannel::NesApuRectangleChannel(int channelNo) :
	NesApuChannel(channelNo) {
}

void NesApuRectangleChannel::reset() {
	NesApuChannel::reset();
	sweepShiftAmount = 0;
	sweepDirection = false;
	sweepUpdateRate = 0;
	sweepEnable = false;
	sweepCounter = 0;
	sweepCarry = false;
	updateSweepPeriod = false;
	squareCounter = 0;
}

/*
4001h - APU Sweep Channel 1 (Rectangle)
4005h - APU Sweep Channel 2 (Rectangle)
  0-2     Sweep right shift amount (S=0..7)
  3       Sweep Direction          (0=[+]Increase, 1=[-]Decrease)
  4-6     Sweep update rate        (N=0..7), NTSC=120Hz/(N+1), PAL=96Hz/(N+1)
  7       Sweep enable             (0=Disable, 1=Enable)

At specified Update Rate, the 11bit Wavelength will be modified as such:  Wavelength = Wavelength +/- (Wavelength SHR S)
  (For Channel 1 Decrease only: minus an additional 1)
  (Ie. in Decrease mode: Channel 1 uses NOT, Channel 2 uses NEG)

Wavelength register will be updated only if all 3 of these conditions are met:  Bit 7 is set (sweeping enabled)
  The shift value (which is S in the formula) does not equal to 0
  The channel's length counter contains a non-zero value

Sweep end: The channel gets silenced, and sweep clock is halted, when:  1) current 11bit wavelength value is less than 008h
  2) new 11bit wavelength would become greater than 7FFh

Note that these conditions pertain regardless of any sweep refresh rate values, or if sweeping is enabled/disabled (via Bit7).
 */
void NesApuRectangleChannel::setSweep(quint8 data) {
	sweepShiftAmount = data & 0x07;
	sweepDirection = data & 0x08;
	sweepUpdateRate = ((data >> 4) & 0x07) + 1;
	sweepEnable = data & 0x80;
}

void NesApuRectangleChannel::clockSweep() {
	if (--sweepCounter <= 0) {
		sweepCounter = sweepUpdateRate;
		if (sweepEnable && sweepShiftAmount > 0 && progTimerMax > 7) {
			sweepCarry = false;
			if (!sweepDirection) {
				progTimerMax += (progTimerMax >> sweepShiftAmount);
				if (progTimerMax > 4095){
					progTimerMax = 4095;
					sweepCarry = true;
				}
			} else {
				progTimerMax -= ((progTimerMax >> sweepShiftAmount) - channelNo);
			}
		}
	}
	if (updateSweepPeriod) {
		updateSweepPeriod = false;
		sweepCounter = sweepUpdateRate;
	}
}

void NesApuRectangleChannel::updateSampleValue() {
	if (sampleCondition) {
		if (!sweepDirection && (progTimerMax + (progTimerMax >> sweepShiftAmount)) > 4095) {
			sampleValue = 0;
		} else {
			sampleValue = masterVolume * dutyLUT[(dutyMode << 3) + squareCounter];
		}
	} else {
		sampleValue = 0;
	}
}

int NesApuRectangleChannel::dutyLUT[32] = {
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 0, 0, 0,
	1, 0, 0, 1, 1, 1, 1, 1
};
