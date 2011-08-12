#include "mapper6.h"
#include <QDataStream>

class NesMapper6Data {
public:
	quint8 irqEnable;
	uint irqCounter;
};

CpuMapper6::CpuMapper6(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper),
	m_ppuMapper(0) {
	d = new NesMapper6Data();
}

CpuMapper6::~CpuMapper6() {
	delete d;
}

void CpuMapper6::reset() {
	NesCpuMemoryMapper::reset();
	m_ppuMapper = static_cast<PpuMapper6 *>(mapper()->ppuMemory());
	setRom16KBank(0, 0);
	setRom16KBank(1, 7);
	if (m_ppuMapper->romSize1KB())
		m_ppuMapper->setRomBank(0);
	else
		m_ppuMapper->setCramBank(0);
	d->irqEnable = 0;
	d->irqCounter = 0;
}

void CpuMapper6::write(quint16 address, quint8 data) {
	if (address >= 0x42FE && address <= 0x4503) {
		switch (address) {
		case 0x42FE:
			if (data & 0x10)
				m_ppuMapper->setMirroring(NesPpuMemoryMapper::SingleHigh);
			else
				m_ppuMapper->setMirroring(NesPpuMemoryMapper::SingleLow);
			break;
		case 0x42FF:
			if (data & 0x10)
				m_ppuMapper->setMirroring(NesPpuMemoryMapper::Horizontal);
			else
				m_ppuMapper->setMirroring(NesPpuMemoryMapper::Vertical);
			break;
		case 0x4501:
			d->irqEnable = 0;
			mapper()->setIrqSignalOut(false);
			break;
		case 0x4502:
			d->irqCounter = (d->irqCounter&0xFF00) | (data<<0);
			break;
		case 0x4503:
			d->irqCounter = (d->irqCounter&0x00FF) | (data<<8);
			d->irqEnable = 0xFF;
			mapper()->setIrqSignalOut(false);
			break;
		default:
			CpuMapper6::write(address, data);
			break;
		}
	} else {
		CpuMapper6::write(address, data);
	}
}

void CpuMapper6::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom16KBank(0, (data & 0x3C) >> 2);
	m_ppuMapper->setCramBank(data & 0x03);
}

PpuMapper6::PpuMapper6(NesMapper *mapper) :
	NesPpuMemoryMapper(mapper),
	d(0) {
}

void PpuMapper6::reset() {
	NesPpuMemoryMapper::reset();
	d = static_cast<CpuMapper6 *>(mapper()->cpuMemory())->d;
}

void PpuMapper6::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if(d->irqEnable) {
		d->irqCounter += 133;
		if (d->irqCounter >= 0xFFFF) {
			d->irqCounter = 0;
			mapper()->setIrqSignalOut(true);
		}
	}
}

void CpuMapper6::save(QDataStream &s) {
	NesCpuMemoryMapper::save(s);
	s << d->irqEnable;
	s << d->irqCounter;
}

bool CpuMapper6::load(QDataStream &s) {
	if (!NesCpuMemoryMapper::load(s))
		return false;
	s >> d->irqEnable;
	s >> d->irqCounter;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(6, "FFE F4XX")
