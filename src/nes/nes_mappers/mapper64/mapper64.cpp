#include "mapper64.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

CpuMapper64::CpuMapper64(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper64::reset() {
	ppuMapper = static_cast<PpuMapper64 *>(mapper()->ppuMapper());

	setRom8KBanks(romSize8KB()-1, romSize8KB()-1, romSize8KB()-1, romSize8KB()-1);

	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);

	reg[0] = reg[1] = reg[2] = 0;

	irq_enable   = 0;
	irq_mode     = 0;
	irq_counter  = 0;
	irq_counter2 = 0;
	irq_latch    = 0;
	irq_reset    = 0;
}

void CpuMapper64::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF003) {
	case 0x8000:
		reg[0] = data&0x0F;
		reg[1] = data&0x40;
		reg[2] = data&0x80;
		break;

	case 0x8001:
		switch (reg[0]) {
		case 0x00:
			if (reg[2]) {
				ppuMapper->setVrom1KBank(4, data+0);
				ppuMapper->setVrom1KBank(5, data+1);
			} else {
				ppuMapper->setVrom1KBank(0, data+0);
				ppuMapper->setVrom1KBank(1, data+1);
			}
			break;
		case 0x01:
			if (reg[2]) {
				ppuMapper->setVrom1KBank(6, data+0);
				ppuMapper->setVrom1KBank(7, data+1);
			} else {
				ppuMapper->setVrom1KBank(2, data+0);
				ppuMapper->setVrom1KBank(3, data+1);
			}
			break;
		case 0x02:
			if (reg[2]) {
				ppuMapper->setVrom1KBank(0, data);
			} else {
				ppuMapper->setVrom1KBank(4, data);
			}
			break;
		case 0x03:
			if (reg[2]) {
				ppuMapper->setVrom1KBank(1, data);
			} else {
				ppuMapper->setVrom1KBank(5, data);
			}
			break;
		case 0x04:
			if (reg[2]) {
				ppuMapper->setVrom1KBank(2, data);
			} else {
				ppuMapper->setVrom1KBank(6, data);
			}
			break;
		case 0x05:
			if (reg[2]) {
				ppuMapper->setVrom1KBank(3, data);
			} else {
				ppuMapper->setVrom1KBank(7, data);
			}
			break;
		case 0x06:
			if (reg[1]) {
				setRom8KBank(5, data);
			} else {
				setRom8KBank(4, data);
			}
			break;
		case 0x07:
			if (reg[1]) {
				setRom8KBank(6, data);
			} else {
				setRom8KBank(5, data);
			}
			break;
		case 0x08:
			ppuMapper->setVrom1KBank(1, data);
			break;
		case 0x09:
			ppuMapper->setVrom1KBank(3, data);
			break;
		case 0x0F:
			if (reg[1]) {
				setRom8KBank(4, data);
			} else {
				setRom8KBank(6, data);
			}
			break;
		}
		break;

	case 0xA000:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 0x01));
		break;

	case 0xC000:
		irq_latch = data;
		if (irq_reset) {
			irq_counter = irq_latch;
		}
		break;
	case 0xC001:
		irq_reset = 0xFF;
		irq_counter = irq_latch;
		irq_mode = data & 0x01;
		break;
	case 0xE000:
		irq_enable = 0;
		if (irq_reset) {
			irq_counter = irq_latch;
		}
		setIrqSignalOut(false);
		break;
	case 0xE001:
		irq_enable = 0xFF;
		if (irq_reset) {
			irq_counter = irq_latch;
		}
		break;
	}
}

void CpuMapper64::clock(uint cycles) {
	if (!irq_mode)
		return;

	irq_counter2 += cycles;
	while( irq_counter2 >= 4) {
		irq_counter2 -= 4;
		if (irq_counter >= 0) {
			irq_counter--;
			if (irq_counter < 0) {
				if (irq_enable)
					setIrqSignalOut(true);
			}
		}
	}
}

PpuMapper64::PpuMapper64(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper64::reset() {
	cpuMapper = static_cast<CpuMapper64 *>(mapper()->cpuMapper());
}

void PpuMapper64::horizontalSync(int scanline) {
	if (cpuMapper->irq_mode)
		return;

	cpuMapper->irq_reset = 0;

	if (scanline < NesPpu::VisibleScreenHeight && cpuMapper->ppu()->registers()->isDisplayOn()) {
		if (cpuMapper->irq_counter >= 0) {
			cpuMapper->irq_counter--;
			if (cpuMapper->irq_counter < 0) {
				if (cpuMapper->irq_enable) {
					cpuMapper->irq_reset = 1;
					cpuMapper->setIrqSignalOut(true);
				}
			}
		}
	}
}

bool CpuMapper64::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << reg[0] << reg[1] << reg[2];
	s << irq_enable;
	s << irq_mode;
	s << irq_counter;
	s << irq_counter2;
	s << irq_latch;
	s << irq_reset;
	return true;
}

bool CpuMapper64::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> reg[0] >> reg[1] >> reg[2];
	s >> irq_enable;
	s >> irq_mode;
	s >> irq_counter;
	s >> irq_counter2;
	s >> irq_latch;
	s >> irq_reset;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(64, "Tengen Rambo-1")
