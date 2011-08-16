#include "mapper65.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

CpuMapper65::CpuMapper65(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper65::reset() {
	ppuMapper = static_cast<PpuMapper65 *>(mapper()->ppuMapper());

	patch = 0;

	// Kaiketsu Yanchamaru 3(J)
	if (disk()->crc() == 0xe30b7f64)
		patch = 1;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);

	irq_enable = 0;
	irq_counter = 0;
}

void CpuMapper65::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		setRom8KBank(4, data);
		break;

	case 0x9000:
		if (!patch)
			ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x40) >> 6));
		break;

	case 0x9001:
		if (patch)
			ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>((data & 0x80) >> 7));
		break;

	case 0x9003:
		if (!patch) {
			irq_enable = data & 0x80;
			setIrqSignalOut(false);
		}
		break;
	case 0x9004:
		if (!patch) {
			irq_counter = irq_latch;
		}
		break;
	case 0x9005:
		if (patch) {
			irq_counter = data<<1;
			irq_enable = data;
			setIrqSignalOut(false);
		} else {
			irq_latch = (irq_latch & 0x00FF)|(data<<8);
		}
		break;

	case 0x9006:
		if (patch) {
			irq_enable = 1;
		} else {
			irq_latch = (irq_latch & 0xFF00)|data;
		}
		break;

	case 0xB000:
	case 0xB001:
	case 0xB002:
	case 0xB003:
	case 0xB004:
	case 0xB005:
	case 0xB006:
	case 0xB007:
		ppuMapper->setVrom1KBank(address & 0x0007, data);
		break;

	case 0xA000:
		setRom8KBank(5, data);
		break;
	case 0xC000:
		setRom8KBank(6, data);
		break;
	}
}

void CpuMapper65::clock(uint cycles) {
	if (!patch) {
		if (irq_enable) {
			if (irq_counter <= 0) {
				setIrqSignalOut(true);
			} else {
				irq_counter -= cycles;
			}
		}
	}
}

PpuMapper65::PpuMapper65(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper65::reset() {
	cpuMapper = static_cast<CpuMapper65 *>(mapper()->cpuMapper());
}

void PpuMapper65::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if (cpuMapper->patch) {
		if (cpuMapper->irq_enable) {
			if (cpuMapper->irq_counter == 0) {
				cpuMapper->setIrqSignalOut(true);
			} else {
				cpuMapper->irq_counter--;
			}
		}
	}
}

bool CpuMapper65::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << irq_enable;
	s << irq_counter;
	s << irq_latch;
	return true;
}

bool CpuMapper65::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> irq_enable;
	s >> irq_counter;
	s >> irq_latch;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(65, "Irem H3001")
