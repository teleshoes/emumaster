#include "mapper17.h"
#include <QDataStream>

class NesMapper17Data {
public:
	quint8 irqEnable;
	int irqCounter;
	int irqLatch;
};

CpuMapper17::CpuMapper17(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
	d = new NesMapper17Data();
}

CpuMapper17::~CpuMapper17() {
	delete d;
}

void CpuMapper17::reset() {
	ppuMapper = static_cast<PpuMapper17 *>(mapper()->ppuMapper());

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
	d->irqEnable = 0;
	d->irqCounter = 0;
}

void CpuMapper17::writeLow(quint16 address, quint8 data) {
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
		d->irqLatch = (d->irqCounter&0x00FF) | (data<<8);
		d->irqCounter = d->irqLatch;
		d->irqEnable = 0xFF;
		break;
	case 0x4504:
	case 0x4505:
	case 0x4506:
	case 0x4507:
		setRom8KBank(address&0x07, data);
		break;

	case 0x4510:
	case 0x4511:
	case 0x4512:
	case 0x4513:
	case 0x4514:
	case 0x4515:
	case 0x4516:
	case 0x4517:
		ppuMapper->setVrom1KBank(address&0x07, data);
		break;

	default:
		CpuMapper17::writeLow(address, data);
		break;
	}
}

void CpuMapper17::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom16KBank(4, (data & 0x3C) >> 2);
	ppuMapper->setCram8KBank(data & 0x03);
}

PpuMapper17::PpuMapper17(NesMapper *mapper) :
	NesPpuMapper(mapper),
	d(0),
	cpuMapper(0) {
}

void PpuMapper17::reset() {
	cpuMapper = static_cast<CpuMapper17 *>(mapper()->cpuMapper());
	d = cpuMapper->d;
}

void PpuMapper17::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if(d->irqEnable) {
		if (d->irqCounter >= 0xFFFF-113) {
			cpuMapper->setIrqSignalOut(true);
			d->irqCounter &= 0xFFFF;
		} else {
			d->irqCounter += 133;
		}
	}
}

bool CpuMapper17::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << d->irqEnable;
	s << d->irqCounter;
	s << d->irqLatch;
	return true;
}

bool CpuMapper17::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> d->irqEnable;
	s >> d->irqCounter;
	s >> d->irqLatch;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(6, "FFE F8XX")
