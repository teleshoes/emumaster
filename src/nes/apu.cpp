#include "apu.h"
#include "apurectanglechannel.h"
#include "aputrianglechannel.h"
#include "apunoisechannel.h"
#include "apudmchannel.h"
#include "machine.h"
#include "cpu.h"
#include <QDataStream>

static const int SampleRate = 44100;

static const int StereoPosLR1 =  80;
static const int StereoPosLR2 = 170;
static const int StereoPosLTR = 100;
static const int StereoPosLNS = 150;
static const int StereoPosLDM = 128;
static const int StereoPosRR1 = 256 - StereoPosLR1;
static const int StereoPosRR2 = 256 - StereoPosLR2;
static const int StereoPosRTR = 256 - StereoPosLTR;
static const int StereoPosRNS = 256 - StereoPosLNS;
static const int StereoPosRDM = 256 - StereoPosLDM;

NesApu nesApu;

static NesApuRectangleChannel	r1ch(0);
static NesApuRectangleChannel	r2ch(1);
static NesApuTriangleChannel	trch(2);
static NesApuNoiseChannel		nsch(3);
static NesApuDMChannel			dmch(4);

static bool frameIrqGenerated;
static bool frameIrqEnable;
static int frameIrqCounterMax;
static int initCounter;
static bool initializingHardware;

static const int SampleBufferSize = 8192;
static u16 sampleBuffer[SampleBufferSize];

static int bufferIndex;
static int bufferBase;
static int perFrame = 1;

static int masterFrameCounter;
static int derivedFrameCounter;
static int countSequence;

static int sampleTimer;
static int frameTime;
static int sampleTimerMax;

static int triValue;

static int smpR1;
static int smpR2;
static int smpTR;
static int smpNS;
static int smpDM;
static int accCount;

static int extraCycles = 0;

static const int RectangleLUTSize = 32 * 16;
static const int TriangleLUTSize = 204 * 16;

static int rectangleLUT[RectangleLUTSize];
static int triangleLUT[TriangleLUTSize];

static int prevSampleL;
static int prevSampleR;
static int smpAccumL;
static int smpAccumR;

static int dcValue;

static bool irqSignal;

static void frameCounterTick();
static void accSample(int cycles);
static void apuSample();

void NesApu::init() {
	int maxRectangle = 0;
	int maxTriangle = 0;
	for (int i = 0; i < RectangleLUTSize; i++) {
		qreal value = 95.52 / (8128.0 / (qreal(i)/16.0) + 100.0);
		value *= 0.98411;
		value *= 50000.0;
		rectangleLUT[i] = value;
		maxRectangle = qMax(maxRectangle, rectangleLUT[i]);
	}
	for (int i = 0; i < TriangleLUTSize; i++) {
		qreal value = 163.67 / (24329.0 / (qreal(i)/16.0) + 100.0);
		value *= 0.98411;
		value *= 50000.0;
		triangleLUT[i] = value;
		maxTriangle = qMax(maxTriangle, triangleLUT[i]);
	}
	dcValue = (maxRectangle + maxTriangle) / 2;
}

static void updateIrqSignal() {
	bool on = ((frameIrqEnable && frameIrqGenerated) || dmch.irqGenerated);
	if (on != irqSignal) {
		irqSignal = on;
		nesCpu.apu_irq_i(on);
	}
}

void NesApu::write(u16 address, u8 data) {
	Q_ASSERT(address <= 0x17);
	switch (address) {
	case 0x00: r1ch.setVolumeDecay(data); break;
	case 0x01: r1ch.setSweep(data); break;
	case 0x02: r1ch.setFrequency(data); break;
	case 0x03: r1ch.setLength(data); break;
	case 0x04: r2ch.setVolumeDecay(data); break;
	case 0x05: r2ch.setSweep(data); break;
	case 0x06: r2ch.setFrequency(data); break;
	case 0x07: r2ch.setLength(data); break;
	case 0x08: trch.setLinearCounter(data); break;
	case 0x09: break;
	case 0x0A: trch.setFrequency(data); break;
	case 0x0B: trch.setLength(data); break;
	case 0x0C: nsch.setVolumeDecay(data); break;
	case 0x0D: break;
	case 0x0E: nsch.setFrequency(data); break;
	case 0x0F: nsch.setLength(data); break;
	case 0x10: dmch.write0x4010(data); break;
	case 0x11: dmch.write0x4011(data); break;
	case 0x12: dmch.write0x4012(data); break;
	case 0x13: dmch.write0x4013(data); break;
	case 0x15:
		r1ch.setEnabled(data & 0x01);
		r2ch.setEnabled(data & 0x02);
		trch.setEnabled(data & 0x04);
		nsch.setEnabled(data & 0x08);
		dmch.setEnabled(data & 0x10);
		if (data && initCounter > 0)
			initializingHardware = true;
		dmch.write0x4015(data);
		break;
	case 0x17:
		countSequence = (data >> 7) & 1;
		masterFrameCounter = 0;
		frameIrqGenerated = false;
		frameIrqEnable = !((data >> 6) & 0x01);
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

u8 NesApu::read(u16 address) {
	Q_ASSERT(address <= 0x17);
	u8 data = 0;
	if (address == 0x15) {
		data |= (						r1ch.lengthStatus() ? 0x01 : 0);
		data |= (						r2ch.lengthStatus() ? 0x02 : 0);
		data |= (						trch.lengthStatus() ? 0x04 : 0);
		data |= (						nsch.lengthStatus() ? 0x08 : 0);
		data |= (						dmch.lengthStatus() ? 0x10 : 0);

		data |= (	  (frameIrqGenerated && frameIrqEnable) ? 0x40 : 0);
		data |= (						  dmch.irqGenerated ? 0x80 : 0);
		frameIrqGenerated = false;
		dmch.irqGenerated = false;
		updateIrqSignal();
	} else if (address == 0x17) {
		if (!frameIrqGenerated)
			data = 0x40;
	}
	return data;
}

int NesApu::fillBuffer(char *stream, int length) {
	length = (length / perFrame) * perFrame;
	if (!length)
		return 0;
	int sampleLength = (bufferIndex - bufferBase + SampleBufferSize) % SampleBufferSize;
	sampleLength = (sampleLength / perFrame) * perFrame;
	if (!sampleLength)
		return 0;
	if (sampleLength > length / 2)
		sampleLength = length / 2;
	if ((bufferBase + sampleLength) >= SampleBufferSize) {
		u32 partialLength = SampleBufferSize - bufferBase;
		memcpy((u16 *)stream, sampleBuffer + bufferBase, partialLength*2);
		bufferBase = 0;
		u32 remainderLength = sampleLength - partialLength;
		memcpy((u16 *)stream + partialLength, sampleBuffer + bufferBase, remainderLength*2);
		bufferBase = remainderLength;
	} else {
		memcpy((s16 *)stream, sampleBuffer + bufferBase, sampleLength*2);
		bufferBase += sampleLength;
	}
	return sampleLength * 2;
}

void NesApu::clockFrameCounter(int nCycles) {
	Q_ASSERT(nCycles > 0);
	if (initCounter > 0) {
		if (initializingHardware) {
			initCounter -= nCycles;
			if (initCounter <= 0)
				initializingHardware = false;
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
	if (nCycles <= 0)
		return;
	dmch.clock(nCycles);
	trch.clock(nCycles);
	r1ch.clock(nCycles);
	r2ch.clock(nCycles);
	nsch.clock(nCycles);
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
		apuSample();
		sampleTimer -= sampleTimerMax;
	}
}

static void frameCounterTick() {
	derivedFrameCounter++;
	if (derivedFrameCounter >= frameIrqCounterMax)
		derivedFrameCounter = 0;
	if(derivedFrameCounter == 1 || derivedFrameCounter == 3) {
		// clock length & sweep:
		r1ch.clockLengthCounter();
		r2ch.clockLengthCounter();
		trch.clockLengthCounter();
		nsch.clockLengthCounter();

		r1ch.clockSweep();
		r2ch.clockSweep();
	}
	if (derivedFrameCounter >= 0 && derivedFrameCounter < 4) {
		// clock linear & decay:
		r1ch.clockEnvelopeDecay();
		r2ch.clockEnvelopeDecay();
		nsch.clockEnvelopeDecay();
		trch.clockLinearCounter();
	}
	if (derivedFrameCounter == 3 && countSequence == 0)
		frameIrqGenerated = true;
}

static void accSample(int cycles) {
	// Special treatment for triangle channel - need to interpolate.
	if (trch.sampleCondition){
		triValue = (trch.progTimerCount << 4) / (trch.progTimerMax + 1);
		if (triValue > 0x10)
			triValue = 0x10;
		if (trch.triangleCounter() >= 0x10)
			triValue = 0x10 - triValue;
		// add non-interpolated sample value:
		triValue += trch.sampleValue;
	}
	// now sample normally:
	if (cycles == 2) {
		smpTR += triValue			<< 1;
		smpDM += dmch.sampleValue	<< 1;
		smpR1 += r1ch.sampleValue	<< 1;
		smpR2 += r2ch.sampleValue	<< 1;
		accCount    += 2;
	} else if (cycles == 4) {
		smpTR += triValue			<< 2;
		smpDM += dmch.sampleValue	<< 2;
		smpR1 += r1ch.sampleValue	<< 2;
		smpR2 += r2ch.sampleValue	<< 2;
		accCount += 4;
	} else {
		smpTR += cycles * triValue;
		smpDM += cycles * dmch.sampleValue;
		smpR1 += cycles * r1ch.sampleValue;
		smpR2 += cycles * r2ch.sampleValue;
		accCount += cycles;
	}
}

static void apuSample() {
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
		smpR1 = r1ch.sampleValue << 4;
		smpR2 = r2ch.sampleValue << 4;
		smpTR = trch.sampleValue     ;
		smpDM = dmch.sampleValue << 4;
	}
	smpNS = nsch.sample();

	int sampleValueL;
	int sampleValueR;

	// left channel:
	int rectangle = (smpR1 * StereoPosLR1 + smpR2 * StereoPosLR2) >> 8;
	int triangle = (3*smpTR * StereoPosLTR + (smpNS<<1) * StereoPosLNS + smpDM*StereoPosLDM) >> 8;
	if (rectangle >= RectangleLUTSize)
		rectangle = RectangleLUTSize-1;
	if (triangle >= TriangleLUTSize)
		triangle = TriangleLUTSize-1;
	sampleValueL = rectangleLUT[rectangle] + triangleLUT[triangle] - dcValue;

	// right channel:
	rectangle = (smpR1 * StereoPosRR1 + smpR2 * StereoPosRR2) >> 8;
	triangle = (3*smpTR * StereoPosRTR + (smpNS<<1) * StereoPosRNS + smpDM*StereoPosRDM) >> 8;
	if (rectangle >= RectangleLUTSize)
		rectangle = RectangleLUTSize-1;
	if (triangle >= TriangleLUTSize)
		triangle = TriangleLUTSize-1;
	sampleValueR = rectangleLUT[rectangle] + triangleLUT[triangle] - dcValue;

	// remove DC from left channel:
	int smpDiffL = sampleValueL - prevSampleL;
	prevSampleL += smpDiffL;
	smpAccumL += smpDiffL - (smpAccumL >> 10);
	sampleValueL = smpAccumL;

	// remove DC from right channel:
	int smpDiffR = sampleValueR - prevSampleR;
	prevSampleR += smpDiffR;
	smpAccumR += smpDiffR - (smpAccumR >> 10);
	sampleValueR = smpAccumR;

	sampleBuffer[bufferIndex+0] = sampleValueL;
	sampleBuffer[bufferIndex+1] = sampleValueR;

	bufferIndex = (bufferIndex + 2) % SampleBufferSize;

	// reset sampled values:
	smpR1 = 0;
	smpR2 = 0;
	smpTR = 0;
	smpDM = 0;
}

void NesApu::reset() {
	r1ch.reset();
	r2ch.reset();
	trch.reset();
	nsch.reset();
	dmch.reset();

	masterFrameCounter = 0;
	derivedFrameCounter = 4;
	countSequence = 0;

	initCounter = 2048;
	initializingHardware = false;

	frameIrqEnable = false;
	frameIrqGenerated = false;
	frameIrqCounterMax = 4;

	bufferIndex = 0;
	bufferBase = 0;
	sampleTimer = 0;
	accCount = 0;

	smpR1 = 0;
	smpR2 = 0;
	smpTR = 0;
	smpNS = 0;
	smpDM = 0;
	triValue = 0;

	prevSampleL = 0;
	prevSampleR = 0;
	smpAccumL = 0;
	smpAccumR = 0;
}

void NesApu::updateMachineType() {
	if (nesSystemType == NES_NTSC) {
		sampleTimerMax = (1024.0 * NES_CPU_NTSC_CLK * NES_NTSC_FRAMERATE) / (SampleRate*60.0);
		frameTime = 14915.0 * NES_NTSC_FRAMERATE / 60.0;
		perFrame = 44100 / 60;
	} else {
		sampleTimerMax = (1024.0 * NES_CPU_PAL_CLK * NES_PAL_FRAMERATE) / (SampleRate*60.0);
		frameTime = 14915.0 * NES_PAL_FRAMERATE / 60.0;
		perFrame = 44100 / 50;
	}
	sampleTimer = 0;
	bufferIndex = 0;
	bufferBase = 0;
}

u8 NesApu::fetchData(u16 address) {
	u8 data = nesCpu.READ(address);
	nesCpu.ADDCYC(4);
	return data;
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(NesApu, 1) \
	STATE_SERIALIZE_SUBCALL_##sl(r1ch) \
	STATE_SERIALIZE_SUBCALL_##sl(r2ch) \
	STATE_SERIALIZE_SUBCALL_##sl(nsch) \
	STATE_SERIALIZE_SUBCALL_##sl(trch) \
	STATE_SERIALIZE_SUBCALL_##sl(dmch) \
	STATE_SERIALIZE_VAR_##sl(frameIrqGenerated) \
	STATE_SERIALIZE_VAR_##sl(frameIrqEnable) \
	STATE_SERIALIZE_VAR_##sl(frameIrqCounterMax) \
	STATE_SERIALIZE_VAR_##sl(initCounter) \
	STATE_SERIALIZE_VAR_##sl(initializingHardware) \
	STATE_SERIALIZE_VAR_##sl(masterFrameCounter) \
	STATE_SERIALIZE_VAR_##sl(derivedFrameCounter) \
	STATE_SERIALIZE_VAR_##sl(countSequence) \
	STATE_SERIALIZE_VAR_##sl(triValue) \
	STATE_SERIALIZE_VAR_##sl(smpR1) \
	STATE_SERIALIZE_VAR_##sl(smpR2) \
	STATE_SERIALIZE_VAR_##sl(smpTR) \
	STATE_SERIALIZE_VAR_##sl(smpNS) \
	STATE_SERIALIZE_VAR_##sl(smpDM) \
	STATE_SERIALIZE_VAR_##sl(accCount) \
	STATE_SERIALIZE_VAR_##sl(extraCycles) \
	STATE_SERIALIZE_VAR_##sl(prevSampleL) \
	STATE_SERIALIZE_VAR_##sl(prevSampleR) \
	STATE_SERIALIZE_VAR_##sl(smpAccumL) \
	STATE_SERIALIZE_VAR_##sl(smpAccumR) \
	STATE_SERIALIZE_VAR_##sl(irqSignal) \
	sampleTimer = 0; \
	bufferIndex = 0; \
	bufferBase = 0; \
STATE_SERIALIZE_END_##sl(NesApu)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
