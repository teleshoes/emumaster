#include "mapper6.h"
#include <QDataStream>

class NesMapper6Data {
public:
	quint8 irqEnable;
	int irqCounter;
};

CpuMapper6::CpuMapper6(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
	d = new NesMapper6Data();
}

CpuMapper6::~CpuMapper6() {
	delete d;
}

void CpuMapper6::reset() {
	ppuMapper = static_cast<PpuMapper6 *>(mapper()->ppuMapper());

	setRom8KBanks(0, 1, 14, 15);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
	else
		ppuMapper->setCram8KBank(0);
	d->irqEnable = 0;
	d->irqCounter = 0;
}

void CpuMapper6::writeLow(quint16 address, quint8 data) {
	switch (address) {
	case 0x42FE:
		if (data & 0x10)
			ppuMapper->setMirroring(NesPpuMapper::SingleHigh);
		else
			ppuMapper->setMirroring(NesPpuMapper::SingleLow);
		break;
	case 0x42FF:
		if (data & 0x10)
			ppuMapper->setMirroring(NesPpuMapper::Horizontal);
		else
			ppuMapper->setMirroring(NesPpuMapper::Vertical);
		break;
	case 0x4501:
		d->irqEnable = 0;
		setIrqSignalOut(false);
		break;
	case 0x4502:
		d->irqCounter = (d->irqCounter&0xFF00) | (data<<0);
		break;
	case 0x4503:
		d->irqCounter = (d->irqCounter&0x00FF) | (data<<8);
		d->irqEnable = 0xFF;
		setIrqSignalOut(false);
		break;
	default:
		CpuMapper6::writeLow(address, data);
		break;
	}
}

void CpuMapper6::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data & 0x3C) >> 2);
	ppuMapper->setCram8KBank(data & 0x03);
}

PpuMapper6::PpuMapper6(NesMapper *mapper) :
	NesPpuMapper(mapper),
	d(0),
	cpuMapper(0) {
}

void PpuMapper6::reset() {
	cpuMapper = static_cast<CpuMapper6 *>(mapper()->cpuMapper());
	d = cpuMapper->d;
}

void PpuMapper6::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if(d->irqEnable) {
		d->irqCounter += 133;
		if (d->irqCounter >= 0xFFFF) {
			d->irqCounter = 0;
			cpuMapper->setIrqSignalOut(true);
		}
	}
}

bool CpuMapper6::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << d->irqEnable;
	s << d->irqCounter;
	return true;
}

bool CpuMapper6::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> d->irqEnable;
	s >> d->irqCounter;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(6, "FFE F4XX")
