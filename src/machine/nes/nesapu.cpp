#include "nesapu.h"
#include "nesmachine.h"
#include "nescpu.h"
#include <QDebug>

NesApu::NesApu(NesCpu *cpu) :
	QObject(cpu),
	m_r1ch(0),
	m_r2ch(1),
	m_trch(2),
	m_nsch(3),
	m_dmch(this, 4),
	stereo(true),
	sampleRate(-1) {
	extraCycles = 0;
	initDACLUTs();
	setMasterVolume(255);
	reset();
}

void NesApu::write(uint address, quint8 data) {
	Q_ASSERT(address <= 0x17);
	Q_ASSERT_X(sampleRate > 0, "NesApu", "set sample rate first");
	switch (address) {
	case 0x00: m_r1ch.setVolumeDecay(data); break;
	case 0x01: m_r1ch.setSweep(data); break;
	case 0x02: m_r1ch.setFrequency(data); break;
	case 0x03: m_r1ch.setLength(data); break;
	case 0x04: m_r2ch.setVolumeDecay(data); break;
	case 0x05: m_r2ch.setSweep(data); break;
	case 0x06: m_r2ch.setFrequency(data); break;
	case 0x07: m_r2ch.setLength(data); break;
	case 0x08: m_trch.setLinearCounter(data); break;
	case 0x09: break;
	case 0x0A: m_trch.setFrequency(data); break;
	case 0x0B: m_trch.setLength(data); break;
	case 0x0C: m_nsch.setVolumeDecay(data); break;
	case 0x0D: break;
	case 0x0E: m_nsch.setFrequency(data); break;
	case 0x0F: m_nsch.setLength(data); break;
	case 0x10: m_dmch.write0x4010(data); break;
	case 0x11: m_dmch.write0x4011(data); break;
	case 0x12: m_dmch.write0x4012(data); break;
	case 0x13: m_dmch.write0x4013(data); break;
	case 0x15:
		m_r1ch.setEnabled(data & 0x01);
		m_r2ch.setEnabled(data & 0x02);
		m_trch.setEnabled(data & 0x04);
		m_nsch.setEnabled(data & 0x08);
		m_dmch.setEnabled(data & 0x10);
		if (data && m_initCounter > 0)
			m_initializingHardware = true;
		m_dmch.write0x4015(data);
		break;
	case 0x17:
		countSequence = (data >> 7) & 1;
		masterFrameCounter = 0;
		m_frameIrqGenerated = false;
		m_frameIrqEnable = !((data >> 6) & 0x01);
		if (countSequence == 0) {
			frameIrqCounterMax = 4;
			derivedFrameCounter = 4;
		} else {
			frameIrqCounterMax = 5;
			derivedFrameCounter = 0;
			frameCounterTick();
		}
		break;
	default:
		break;
	}
}

quint8 NesApu::read(uint address) {
	Q_ASSERT(address <= 0x17);
	Q_ASSERT_X(sampleRate > 0, "NesApu", "set sample rate first");
	if (address == 0x15) {
		quint8 result = 0;
		result |= (						m_r1ch.lengthStatus() ? 0x01 : 0);
		result |= (						m_r2ch.lengthStatus() ? 0x02 : 0);
		result |= (						m_trch.lengthStatus() ? 0x04 : 0);
		result |= (						m_nsch.lengthStatus() ? 0x08 : 0);
		result |= (						m_dmch.lengthStatus() ? 0x10 : 0);

		result |= ( (m_frameIrqGenerated && m_frameIrqEnable) ? 0x40 : 0);
		result |= (						  m_dmch.irqGenerated ? 0x80 : 0);
		m_frameIrqGenerated = false;
		m_dmch.irqGenerated = false;
		updateIrqSignal();
		return result;
	}
	return 0;
}

void NesApu::setChannelUserEnabled(int channelNo, bool on) {
	switch (channelNo) {
	case 0: m_r1ch.setUserEnabled(on); break;
	case 1: m_r2ch.setUserEnabled(on); break;
	case 2: m_trch.setUserEnabled(on); break;
	case 3: m_nsch.setUserEnabled(on); break;
	case 4: m_dmch.setUserEnabled(on); break;
	default: Q_ASSERT(false); break;
	}
}

void NesApu::clockFrameCounter(int nCycles) {
	Q_ASSERT(nCycles > 0);
	Q_ASSERT_X(sampleRate > 0, "NesApu", "set sample rate first");
	if (m_initCounter > 0) {
		if (m_initializingHardware) {
			m_initCounter -= nCycles;
			if (m_initCounter <= 0)
				m_initializingHardware = false;
			return;
		}
	}
	// don't process ticks beyond next sampling
	nCycles += extraCycles;
	int maxCycles = sampleTimerMax - sampleTimer;
	if ((nCycles << 10) > maxCycles) {
		extraCycles = ((nCycles << 10) - maxCycles) >> 10;
		nCycles -= extraCycles;
	} else {
		extraCycles = 0;
	}
	if (m_dmch.enable) {
		m_dmch.shiftCounter -= (nCycles << 3);
		while (m_dmch.shiftCounter <= 0 && m_dmch.dmaFrequency > 0) {
			m_dmch.shiftCounter += m_dmch.dmaFrequency;
			m_dmch.clock();
			updateIrqSignal();
		}
	}
	if (m_trch.progTimerMax > 0) {
		m_trch.progTimerCount -= nCycles;
		while (m_trch.progTimerCount <= 0) {
			m_trch.progTimerCount += m_trch.progTimerMax + 1;
			if (m_trch.linearCounter > 0 && m_trch.lengthCounter > 0) {
				m_trch.triangleCounter++;
				m_trch.triangleCounter &= 0x1F;
				if (m_trch.enable) {
					if (m_trch.triangleCounter >= 0x10)
						m_trch.sampleValue = (m_trch.triangleCounter & 0xF);
					else
						m_trch.sampleValue = 0xF - (m_trch.triangleCounter & 0xF);
					m_trch.sampleValue <<= 4;
				}
			}
		}
	}
	m_r1ch.progTimerCount -= nCycles;
	if (m_r1ch.progTimerCount <= 0) {
		m_r1ch.progTimerCount += (m_r1ch.progTimerMax + 1) << 1;
		m_r1ch.squareCounter++;
		m_r1ch.squareCounter &= 0x7;
		m_r1ch.updateSampleValue();
	}
	m_r2ch.progTimerCount -= nCycles;
	if (m_r2ch.progTimerCount <= 0) {
		m_r2ch.progTimerCount += (m_r2ch.progTimerMax + 1) << 1;
		m_r2ch.squareCounter++;
		m_r2ch.squareCounter &= 0x7;
		m_r2ch.updateSampleValue();
	}
	m_nsch.step(nCycles);
	updateIrqSignal();
	// clock frame counter at double CPU speed:
	masterFrameCounter += nCycles << 1;
	if (masterFrameCounter >= frameTime) {
		// 240Hz tick:
		masterFrameCounter -= frameTime;
		frameCounterTick();
	}
	accSample(nCycles);
	sampleTimer += nCycles << 10;
	if (sampleTimer >= sampleTimerMax) {
		sample();
		sampleTimer -= sampleTimerMax;
	}
}

void NesApu::frameCounterTick() {
	derivedFrameCounter++;
	if (derivedFrameCounter >= frameIrqCounterMax)
		derivedFrameCounter = 0;
	if(derivedFrameCounter == 1 || derivedFrameCounter == 3) {
		// clock length & sweep:
		m_r1ch.clockLengthCounter();
		m_r2ch.clockLengthCounter();
		m_trch.clockLengthCounter();
		m_nsch.clockLengthCounter();

		m_r1ch.clockSweep();
		m_r2ch.clockSweep();
	}
	if (derivedFrameCounter >= 0 && derivedFrameCounter < 4) {
		// clock linear & decay:
		m_r1ch.clockEnvelopeDecay();
		m_r2ch.clockEnvelopeDecay();
		m_nsch.clockEnvelopeDecay();
		m_trch.clockLinearCounter();
	}
	if (derivedFrameCounter == 3 && countSequence == 0)
		m_frameIrqGenerated = true;
}

void NesApu::accSample(int cycles) {
	// Special treatment for triangle channel - need to interpolate.
	if (m_trch.sampleCondition){
		triValue = (m_trch.progTimerCount << 4) / (m_trch.progTimerMax + 1);
		if (triValue > 16)
			triValue = 16;
		if (m_trch.triangleCounter >= 16) {
			triValue = 16 - triValue;
		}
		// add non-interpolated sample value:
		triValue += m_trch.sampleValue;
	}
	// TODO test performance, maybe remove conditionals
	// now sample normally:
	if (cycles == 2) {
		smpTR += triValue			<< 1;
		smpDM += m_dmch.sampleValue	<< 1;
		smpR1 += m_r1ch.sampleValue	<< 1;
		smpR2 += m_r2ch.sampleValue	<< 1;
		accCount    += 2;
	} else if (cycles == 4) {
		smpTR += triValue			<< 2;
		smpDM += m_dmch.sampleValue	<< 2;
		smpR1 += m_r1ch.sampleValue	<< 2;
		smpR2 += m_r2ch.sampleValue	<< 2;
		accCount += 4;
	} else {
		smpTR += cycles * triValue;
		smpDM += cycles * m_dmch.sampleValue;
		smpR1 += cycles * m_r1ch.sampleValue;
		smpR2 += cycles * m_r2ch.sampleValue;
		accCount += cycles;
	}
}

void NesApu::sample() {
	if (accCount > 0) {
		smpR1 <<= 4;
		smpR1 /= accCount;
		smpR2 <<= 4;
		smpR2 /= accCount;
		smpTR /= accCount;
		smpDM <<= 4;
		smpDM /= accCount;
		accCount = 0;
	} else {
		smpR1 = m_r1ch.sampleValue << 4;
		smpR2 = m_r2ch.sampleValue << 4;
		smpTR = m_trch.sampleValue     ;
		smpDM = m_dmch.sampleValue << 4;
	}
	// TODO move to noise channel
	smpNS = (m_nsch.accValue << 4) / m_nsch.accCount;
	m_nsch.accValue = smpNS >> 4;
	m_nsch.accCount = 1;
	if (stereo) {
		// left channel:
		int sq_index = (smpR1 * stereoPosLSquare1 + smpR2 * stereoPosLSquare2) >> 8;
		int tnd_index = (3*smpTR * stereoPosLTriangle + (smpNS<<1)   * stereoPosLNoise + smpDM*stereoPosLDMC) >> 8;
		if (sq_index >= SquareTableSize)
			sq_index = SquareTableSize-1;
		if (tnd_index >= TndTableSize)
			tnd_index = TndTableSize-1;
		sampleValueL = square_table[sq_index] + tnd_table[tnd_index] - dcValue;
		// right channel:
		sq_index = (smpR1 * stereoPosRSquare1 + smpR2 * stereoPosRSquare2) >> 8;
		tnd_index = (3*smpTR * stereoPosRTriangle + (smpNS<<1) * stereoPosRNoise + smpDM*stereoPosRDMC) >> 8;
		if (sq_index >= SquareTableSize)
			sq_index = SquareTableSize-1;
		if (tnd_index >= TndTableSize)
			tnd_index = TndTableSize-1;
		sampleValueR = square_table[sq_index] + tnd_table[tnd_index] - dcValue;
	} else {
		// mono sound
		int sq_index = smpR1 + smpR2;
		int tnd_index = 3*smpTR + 2*smpNS + smpDM;
		if (sq_index >= SquareTableSize)
			sq_index  = SquareTableSize-1;
		if(tnd_index >= TndTableSize)
			tnd_index = TndTableSize-1;
		sampleValueL = 3*(square_table[sq_index] + tnd_table[tnd_index] - dcValue);
		sampleValueL >>= 2;
	}
	// remove DC from left channel:
	smpDiffL     = sampleValueL - prevSampleL;
	prevSampleL += smpDiffL;
	smpAccumL   += smpDiffL - (smpAccumL >> 10);
	sampleValueL = smpAccumL;

	if (stereo) {
		// remove DC from right channel:
		smpDiffR 	 = sampleValueR - prevSampleR;
		prevSampleR += smpDiffR;
		smpAccumR 	+= smpDiffR - (smpAccumR >> 10);
		sampleValueR = smpAccumR;

		if(bufferIndex+4 < SampleBufferSize){
			sampleBuffer[bufferIndex++] = (sampleValueL   ) & 0xFF;
			sampleBuffer[bufferIndex++] = (sampleValueL>>8) & 0xFF;
			sampleBuffer[bufferIndex++] = (sampleValueR   ) & 0xFF;
			sampleBuffer[bufferIndex++] = (sampleValueR>>8) & 0xFF;
		}
	} else {
		if (bufferIndex+2 < SampleBufferSize) {
			sampleBuffer[bufferIndex++] = (sampleValueL   ) & 0xFF;
			sampleBuffer[bufferIndex++] = (sampleValueL>>8) & 0xFF;
		}
	}
	// reset sampled values:
	smpR1 = 0;
	smpR2 = 0;
	smpTR = 0;
	smpDM = 0;
}

void NesApu::setSampleRate(int rate) {
	if (sampleRate == rate)
		return;
	sampleRate = rate;
	updateFrameRate();
}

void NesApu::setStereoEnabled(bool on) {
	if (stereo == on)
		return;
	stereo = on;
	sampleTimer = 0;
	bufferIndex = 0;
}

void NesApu::initDACLUTs() {
	int max_sqr = 0;
	int max_tnd = 0;
	for (int i = 0; i < SquareTableSize; i++) {
		qreal value = 95.52 / (8128.0 / (qreal(i)/16.0) + 100.0);
		value *= 0.98411;
		value *= 50000.0;
		square_table[i] = value;
		max_sqr = qMax(max_sqr, square_table[i]);
	}
	for (int i = 0; i < TndTableSize; i++) {
		qreal value = 163.67 / (24329.0 / (qreal(i)/16.0) + 100.0);
		value *= 0.98411;
		value *= 50000.0;
		tnd_table[i] = value;
		max_tnd = qMax(max_tnd, tnd_table[i]);
	}
	int dacRange = max_sqr + max_tnd;
	dcValue = dacRange / 2;
}

void NesApu::updateIrqSignal() {
	bool on = ((m_frameIrqEnable && m_frameIrqGenerated) || m_dmch.irqGenerated);
	if (on != m_irqSignal) {
		m_irqSignal = on;
		emit request_irq_o(on);
	}
}

void NesApu::setMasterVolume(int value) {
	masterVolume = qBound(0, value, 255);
	stereoPosLSquare1  = ( 80 * masterVolume) >> 8;
	stereoPosLSquare2  = (170 * masterVolume) >> 8;
	stereoPosLTriangle = (100 * masterVolume) >> 8;
	stereoPosLNoise    = (150 * masterVolume) >> 8;
	stereoPosLDMC      = (128 * masterVolume) >> 8;

	stereoPosRSquare1	= masterVolume - stereoPosLSquare1;
	stereoPosRSquare2	= masterVolume - stereoPosLSquare2;
	stereoPosRTriangle	= masterVolume - stereoPosLTriangle;
	stereoPosRNoise		= masterVolume - stereoPosLNoise;
	stereoPosRDMC		= masterVolume - stereoPosLDMC;
}

void NesApu::reset() {
	m_r1ch.setEnabled(false);
	m_r2ch.setEnabled(false);
	m_trch.setEnabled(false);
	m_nsch.setEnabled(false);
	m_dmch.setEnabled(false);

	m_r1ch.reset();
	m_r2ch.reset();
	m_trch.reset();
	m_nsch.reset();
	m_dmch.reset();

	masterFrameCounter = 0;
	derivedFrameCounter = 4;
	countSequence = 0;
	sampleCount = 0;

	m_initCounter = 2048;
	m_initializingHardware = false;

	m_frameIrqEnable = false;
	m_frameIrqGenerated = false;
	frameIrqCounterMax = 4;

	bufferIndex = 0;
	sampleTimer = 0;
	accCount = 0;

	smpR1 = 0;
	smpR2 = 0;
	smpTR = 0;
	smpNS = 0;
	smpDM = 0;
	triValue = 0;

	sampleValueL = 0;
	sampleValueR = 0;
	prevSampleL = 0;
	prevSampleR = 0;
	smpAccumL = 0;
	smpAccumR = 0;
	smpDiffL = 0;
	smpDiffR = 0;
}

void NesApu::updateFrameRate() {
	if (machine()->type() == NesMachine::NTSC) {
		sampleTimerMax = (1024.0 * NES_CPU_NTSC_CLK * NES_NTSC_FRAMERATE) / (sampleRate*60.0);
		frameTime = 14915.0 * NES_NTSC_FRAMERATE / 60.0;
	} else {
		sampleTimerMax = (1024.0 * NES_CPU_PAL_CLK * NES_PAL_FRAMERATE) / (sampleRate*60.0);
		frameTime = 14915.0 * NES_PAL_FRAMERATE / 60.0;
	}
	sampleTimer = 0;
	bufferIndex = 0;
}

quint8 NesApu::fetchData(uint address) {
	NesCpu *cpu = machine()->cpu();
	quint8 data = cpu->read(address);
	cpu->stoleCycles(4);
	return data;
}

NesCpu *NesApu::cpu() const
{ return static_cast<NesCpu *>(parent()); }
NesMachine *NesApu::machine() const
{ return cpu()->machine(); }

const char *NesApu::grabBuffer(int *size) {
	*size = bufferIndex;
	bufferIndex = 0;
	return sampleBuffer;
}
