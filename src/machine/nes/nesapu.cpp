#include "nesapu.h"
#include "nesmachine.h"
#include "nescpu.h"
#include <QDataStream>

NesApu::NesApu(NesCpu *cpu) :
	QObject(cpu),
	m_r1ch(0),
	m_r2ch(1),
	m_trch(2),
	m_nsch(3),
	m_dmch(this, 4),
	m_sampleRate(-1),
	m_stereo(true) {
	m_extraCycles = 0;
	initDACLUTs();
}

void NesApu::write(quint16 address, quint8 data) {
	Q_ASSERT(address <= 0x17);
	Q_ASSERT_X(m_sampleRate > 0, "NesApu", "set sample rate first");
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
		m_countSequence = (data >> 7) & 1;
		m_masterFrameCounter = 0;
		m_frameIrqGenerated = false;
		m_frameIrqEnable = !((data >> 6) & 0x01);
		if (m_countSequence == 0) {
			m_frameIrqCounterMax = 4;
			m_derivedFrameCounter = 4;
		} else {
			m_frameIrqCounterMax = 5;
			m_derivedFrameCounter = 0;
			frameCounterTick();
		}
		break;
	default:
		break;
	}
}

quint8 NesApu::read(quint16 address) {
	Q_ASSERT(address <= 0x17);
	Q_ASSERT_X(m_sampleRate > 0, "NesApu", "set sample rate first");
	quint8 data = 0;
	if (address == 0x15) {
		data |= (						m_r1ch.lengthStatus() ? 0x01 : 0);
		data |= (						m_r2ch.lengthStatus() ? 0x02 : 0);
		data |= (						m_trch.lengthStatus() ? 0x04 : 0);
		data |= (						m_nsch.lengthStatus() ? 0x08 : 0);
		data |= (						m_dmch.lengthStatus() ? 0x10 : 0);

		data |= (	(m_frameIrqGenerated && m_frameIrqEnable) ? 0x40 : 0);
		data |= (						  m_dmch.irqGenerated ? 0x80 : 0);
		m_frameIrqGenerated = false;
		m_dmch.irqGenerated = false;
		updateIrqSignal();
	} else if (address == 0x17) {
		if (!m_frameIrqGenerated)
			data = 0x40;
	}
	return data;
}

void NesApu::clockFrameCounter(int nCycles) {
	Q_ASSERT(nCycles > 0);
	Q_ASSERT_X(m_sampleRate > 0, "NesApu", "set sample rate first");
	if (m_initCounter > 0) {
		if (m_initializingHardware) {
			m_initCounter -= nCycles;
			if (m_initCounter <= 0)
				m_initializingHardware = false;
			return;
		}
	}
	// don't process ticks beyond next sampling
	nCycles += m_extraCycles;
	int maxCycles = m_sampleTimerMax - m_sampleTimer;
	if ((nCycles << 10) > maxCycles) {
		m_extraCycles = ((nCycles << 10) - maxCycles) >> 10;
		nCycles -= m_extraCycles;
	} else {
		m_extraCycles = 0;
	}
	// TODO why ??? GALAGA
	if (nCycles <= 0)
		return;
	m_dmch.clock(nCycles);
	m_trch.clock(nCycles);
	m_r1ch.clock(nCycles);
	m_r2ch.clock(nCycles);
	m_nsch.clock(nCycles);
	updateIrqSignal();
	// clock frame counter at double CPU speed:
	m_masterFrameCounter += nCycles << 1;
	if (m_masterFrameCounter >= m_frameTime) {
		// 240Hz tick:
		m_masterFrameCounter -= m_frameTime;
		frameCounterTick();
	}
	accSample(nCycles);
	m_sampleTimer += nCycles << 10;
	if (m_sampleTimer >= m_sampleTimerMax) {
		sample();
		m_sampleTimer -= m_sampleTimerMax;
	}
}

void NesApu::frameCounterTick() {
	m_derivedFrameCounter++;
	if (m_derivedFrameCounter >= m_frameIrqCounterMax)
		m_derivedFrameCounter = 0;
	if(m_derivedFrameCounter == 1 || m_derivedFrameCounter == 3) {
		// clock length & sweep:
		m_r1ch.clockLengthCounter();
		m_r2ch.clockLengthCounter();
		m_trch.clockLengthCounter();
		m_nsch.clockLengthCounter();

		m_r1ch.clockSweep();
		m_r2ch.clockSweep();
	}
	if (m_derivedFrameCounter >= 0 && m_derivedFrameCounter < 4) {
		// clock linear & decay:
		m_r1ch.clockEnvelopeDecay();
		m_r2ch.clockEnvelopeDecay();
		m_nsch.clockEnvelopeDecay();
		m_trch.clockLinearCounter();
	}
	if (m_derivedFrameCounter == 3 && m_countSequence == 0)
		m_frameIrqGenerated = true;
}

void NesApu::accSample(int cycles) {
	// Special treatment for triangle channel - need to interpolate.
	if (m_trch.sampleCondition){
		m_triValue = (m_trch.progTimerCount << 4) / (m_trch.progTimerMax + 1);
		if (m_triValue > 0x10)
			m_triValue = 0x10;
		if (m_trch.triangleCounter() >= 0x10)
			m_triValue = 0x10 - m_triValue;
		// add non-interpolated sample value:
		m_triValue += m_trch.sampleValue;
	}
	// TODO test performance, maybe remove conditionals
	// now sample normally:
	if (cycles == 2) {
		m_smpTR += m_triValue			<< 1;
		m_smpDM += m_dmch.sampleValue	<< 1;
		m_smpR1 += m_r1ch.sampleValue	<< 1;
		m_smpR2 += m_r2ch.sampleValue	<< 1;
		m_accCount    += 2;
	} else if (cycles == 4) {
		m_smpTR += m_triValue			<< 2;
		m_smpDM += m_dmch.sampleValue	<< 2;
		m_smpR1 += m_r1ch.sampleValue	<< 2;
		m_smpR2 += m_r2ch.sampleValue	<< 2;
		m_accCount += 4;
	} else {
		m_smpTR += cycles * m_triValue;
		m_smpDM += cycles * m_dmch.sampleValue;
		m_smpR1 += cycles * m_r1ch.sampleValue;
		m_smpR2 += cycles * m_r2ch.sampleValue;
		m_accCount += cycles;
	}
}

void NesApu::sample() {
	if (m_accCount > 0) {
		m_smpR1 <<= 4;
		m_smpR1 /= m_accCount;
		m_smpR2 <<= 4;
		m_smpR2 /= m_accCount;
		m_smpTR /= m_accCount;
		m_smpDM <<= 4;
		m_smpDM /= m_accCount;
		m_accCount = 0;
	} else {
		m_smpR1 = m_r1ch.sampleValue << 4;
		m_smpR2 = m_r2ch.sampleValue << 4;
		m_smpTR = m_trch.sampleValue     ;
		m_smpDM = m_dmch.sampleValue << 4;
	}
	m_smpNS = m_nsch.sample();

	int sampleValueL;
	int sampleValueR;
	if (m_stereo) {
		// left channel:
		int rectangle = (m_smpR1 * StereoPosLR1 + m_smpR2 * StereoPosLR2) >> 8;
		int triangle = (3*m_smpTR * StereoPosLTR + (m_smpNS<<1) * StereoPosLNS + m_smpDM*StereoPosLDM) >> 8;
		if (rectangle >= RectangleLUTSize)
			rectangle = RectangleLUTSize-1;
		if (triangle >= TriangleLUTSize)
			triangle = TriangleLUTSize-1;
		sampleValueL = m_rectangleLUT[rectangle] + m_triangleLUT[triangle] - m_dcValue;
		// right channel:
		rectangle = (m_smpR1 * StereoPosRR1 + m_smpR2 * StereoPosRR2) >> 8;
		triangle = (3*m_smpTR * StereoPosRTR + (m_smpNS<<1) * StereoPosRNS + m_smpDM*StereoPosRDM) >> 8;
		if (rectangle >= RectangleLUTSize)
			rectangle = RectangleLUTSize-1;
		if (triangle >= TriangleLUTSize)
			triangle = TriangleLUTSize-1;
		sampleValueR = m_rectangleLUT[rectangle] + m_triangleLUT[triangle] - m_dcValue;
	} else {
		int rectangle = m_smpR1 + m_smpR2;
		int triangle = 3*m_smpTR + 2*m_smpNS + m_smpDM;
		if (rectangle >= RectangleLUTSize)
			rectangle  = RectangleLUTSize-1;
		if(triangle >= TriangleLUTSize)
			triangle = TriangleLUTSize-1;
		sampleValueL = 3*(m_rectangleLUT[rectangle] + m_triangleLUT[triangle] - m_dcValue);
		sampleValueL >>= 2;
	}
	// remove DC from left channel:
	int smpDiffL = sampleValueL - m_prevSampleL;
	m_prevSampleL += smpDiffL;
	m_smpAccumL += smpDiffL - (m_smpAccumL >> 10);
	sampleValueL = m_smpAccumL;

	if (m_stereo) {
		// remove DC from right channel:
		int smpDiffR = sampleValueR - m_prevSampleR;
		m_prevSampleR += smpDiffR;
		m_smpAccumR += smpDiffR - (m_smpAccumR >> 10);
		sampleValueR = m_smpAccumR;

		if (m_bufferIndex+4 < SampleBufferSize) {
			m_sampleBuffer[m_bufferIndex++] = (sampleValueL   ) & 0xFF;
			m_sampleBuffer[m_bufferIndex++] = (sampleValueL>>8) & 0xFF;
			m_sampleBuffer[m_bufferIndex++] = (sampleValueR   ) & 0xFF;
			m_sampleBuffer[m_bufferIndex++] = (sampleValueR>>8) & 0xFF;
		}
	} else {
		if (m_bufferIndex+2 < SampleBufferSize) {
			m_sampleBuffer[m_bufferIndex++] = (sampleValueL   ) & 0xFF;
			m_sampleBuffer[m_bufferIndex++] = (sampleValueL>>8) & 0xFF;
		}
	}
	// reset sampled values:
	m_smpR1 = 0;
	m_smpR2 = 0;
	m_smpTR = 0;
	m_smpDM = 0;
}

void NesApu::setSampleRate(int rate) {
	if (m_sampleRate == rate)
		return;
	m_sampleRate = rate;
	updateFrameRate();
}

void NesApu::setStereoEnabled(bool on) {
	if (m_stereo == on)
		return;
	m_stereo = on;
	m_sampleTimer = 0;
	m_bufferIndex = 0;
}

void NesApu::initDACLUTs() {
	int maxRectangle = 0;
	int maxTriangle = 0;
	for (int i = 0; i < RectangleLUTSize; i++) {
		qreal value = 95.52 / (8128.0 / (qreal(i)/16.0) + 100.0);
		value *= 0.98411;
		value *= 50000.0;
		m_rectangleLUT[i] = value;
		maxRectangle = qMax(maxRectangle, m_rectangleLUT[i]);
	}
	for (int i = 0; i < TriangleLUTSize; i++) {
		qreal value = 163.67 / (24329.0 / (qreal(i)/16.0) + 100.0);
		value *= 0.98411;
		value *= 50000.0;
		m_triangleLUT[i] = value;
		maxTriangle = qMax(maxTriangle, m_triangleLUT[i]);
	}
	m_dcValue = (maxRectangle + maxTriangle) / 2;
}

void NesApu::updateIrqSignal() {
	bool on = ((m_frameIrqEnable && m_frameIrqGenerated) || m_dmch.irqGenerated);
	if (on != m_irqSignal) {
		m_irqSignal = on;
		emit request_irq_o(on);
	}
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

	m_masterFrameCounter = 0;
	m_derivedFrameCounter = 4;
	m_countSequence = 0;

	m_initCounter = 2048;
	m_initializingHardware = false;

	m_frameIrqEnable = false;
	m_frameIrqGenerated = false;
	m_frameIrqCounterMax = 4;

	m_bufferIndex = 0;
	m_sampleTimer = 0;
	m_accCount = 0;

	m_smpR1 = 0;
	m_smpR2 = 0;
	m_smpTR = 0;
	m_smpNS = 0;
	m_smpDM = 0;
	m_triValue = 0;

	m_prevSampleL = 0;
	m_prevSampleR = 0;
	m_smpAccumL = 0;
	m_smpAccumR = 0;
}

void NesApu::updateFrameRate() {
	if (machine()->type() == NesMachine::NTSC) {
		m_sampleTimerMax = (1024.0 * NES_CPU_NTSC_CLK * NES_NTSC_FRAMERATE) / (m_sampleRate*60.0);
		m_frameTime = 14915.0 * NES_NTSC_FRAMERATE / 60.0;
	} else {
		m_sampleTimerMax = (1024.0 * NES_CPU_PAL_CLK * NES_PAL_FRAMERATE) / (m_sampleRate*60.0);
		m_frameTime = 14915.0 * NES_PAL_FRAMERATE / 60.0;
	}
	m_sampleTimer = 0;
	m_bufferIndex = 0;
}

quint8 NesApu::fetchData(quint16 address) {
	NesCpu *cpu = machine()->cpu();
	quint8 data = cpu->read(address);
	cpu->ADDCYC(4);
	return data;
}

NesCpu *NesApu::cpu() const
{ return static_cast<NesCpu *>(parent()); }
NesMachine *NesApu::machine() const
{ return cpu()->machine(); }

void NesApu::updateMachineType()
{ updateFrameRate(); }

void NesApu::setChannelUserEnabled(int channelNo, bool on) {
	if (isChannelUserEnabled(channelNo) == on)
		return;
	switch (channelNo) {
	case 0: m_r1ch.setUserEnabled(on); break;
	case 1: m_r2ch.setUserEnabled(on); break;
	case 2: m_trch.setUserEnabled(on); break;
	case 3: m_nsch.setUserEnabled(on); break;
	case 4: m_dmch.setUserEnabled(on); break;
	default: Q_ASSERT(false); break;
	}
	emit channelUserEnableChanged();
}

bool NesApu::isChannelUserEnabled(int channelNo) const {
	switch (channelNo) {
	case 0: return m_r1ch.isUserEnabled(); break;
	case 1: return m_r2ch.isUserEnabled(); break;
	case 2: return m_trch.isUserEnabled(); break;
	case 3: return m_nsch.isUserEnabled(); break;
	case 4: return m_dmch.isUserEnabled(); break;
	default: Q_ASSERT(false); return false; break;
	}
}

bool NesApu::isRectangle1Enabled() const
{ return isChannelUserEnabled(0); }
void NesApu::setRectangle1Enabled(bool on)
{ setChannelUserEnabled(0, on); }

bool NesApu::isRectangle2Enabled() const
{ return isChannelUserEnabled(1); }
void NesApu::setRectangle2Enabled(bool on)
{ setChannelUserEnabled(1, on); }

bool NesApu::isTriangleEnabled() const
{ return isChannelUserEnabled(2); }
void NesApu::setTriangleEnabled(bool on)
{ setChannelUserEnabled(2, on); }

bool NesApu::isNoiseEnabled() const
{ return isChannelUserEnabled(3); }
void NesApu::setNoiseEnabled(bool on)
{ setChannelUserEnabled(3, on); }

bool NesApu::isDmcEnabled() const
{ return isChannelUserEnabled(4); }
void NesApu::setDmcEnabled(bool on)
{ setChannelUserEnabled(4, on); }

bool NesApu::save(QDataStream &s) {
	if (!m_r1ch.save(s))
		return false;
	if (!m_r2ch.save(s))
		return false;
	if (!m_nsch.save(s))
		return false;
	if (!m_trch.save(s))
		return false;
	if (!m_dmch.save(s))
		return false;

	s << m_frameIrqGenerated;
	s << m_frameIrqEnable;
	s << m_frameIrqCounterMax;

	s << m_initCounter;
	s << m_initializingHardware;
	s << m_masterFrameCounter;
	s << m_derivedFrameCounter;
	s << m_countSequence;

	s << m_sampleTimer;

	s << m_triValue;

	s << m_smpR1;
	s << m_smpR2;
	s << m_smpTR;
	s << m_smpNS;
	s << m_smpDM;
	s << m_accCount;

	s << m_extraCycles;

	s << m_prevSampleL;
	s << m_prevSampleR;
	s << m_smpAccumL;
	s << m_smpAccumR;

	s << m_irqSignal;
	return true;
}

bool NesApu::load(QDataStream &s) {
	m_sampleTimer = 0;
	m_bufferIndex = 0;

	if (!m_r1ch.load(s))
		return false;
	if (!m_r2ch.load(s))
		return false;
	if (!m_nsch.load(s))
		return false;
	if (!m_trch.load(s))
		return false;
	if (!m_dmch.load(s))
		return false;

	s >> m_frameIrqGenerated;
	s >> m_frameIrqEnable;
	s >> m_frameIrqCounterMax;

	s >> m_sampleTimer;

	s >> m_initCounter;
	s >> m_initializingHardware;
	s >> m_masterFrameCounter;
	s >> m_derivedFrameCounter;
	s >> m_countSequence;

	s >> m_triValue;

	s >> m_smpR1;
	s >> m_smpR2;
	s >> m_smpTR;
	s >> m_smpNS;
	s >> m_smpDM;
	s >> m_accCount;

	s >> m_extraCycles;

	s >> m_prevSampleL;
	s >> m_prevSampleR;
	s >> m_smpAccumL;
	s >> m_smpAccumR;

	s >> m_irqSignal;
	return true;
}
