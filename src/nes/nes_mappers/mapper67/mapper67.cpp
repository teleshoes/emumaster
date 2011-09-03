#include "mapper67.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

CpuMapper67::CpuMapper67(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper67::reset() {
	ppuMapper = mapper()->ppuMapper();

	irq_enable = 0;
	irq_toggle = 0;
	irq_counter = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	ppuMapper->setVrom4KBank(0, 0);
	ppuMapper->setVrom4KBank(4, ppuMapper->vromSize4KB()-1);

	quint32 crc = disk()->crc();

	if (crc == 0x7f2a04bf) // For Fantasy Zone 2(J)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
}

void CpuMapper67::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF800) {
	case 0x8800:
		ppuMapper->setVrom2KBank(0, data);
		break;
	case 0x9800:
		ppuMapper->setVrom2KBank(2, data);
		break;
	case 0xA800:
		ppuMapper->setVrom2KBank(4, data);
		break;
	case 0xB800:
		ppuMapper->setVrom2KBank(6, data);
		break;

	case 0xC800:
		if (!irq_toggle) {
			irq_counter = (irq_counter&0x00FF) | (data<<8);
		} else {
			irq_counter = (irq_counter&0xFF00) | data;
		}
		irq_toggle ^= 1;
		setIrqSignalOut(false);
		break;
	case 0xD800:
		irq_enable = data & 0x10;
		irq_toggle = 0;
		setIrqSignalOut(false);
		break;

	case 0xE800:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 0x03));
		break;

	case 0xF800:
		setRom16KBank(4, data);
		break;
	}
}

void CpuMapper67::clock(uint cycles) {
	if (irq_enable) {
		if ((irq_counter -= cycles) <= 0) {
			irq_enable = 0;
			irq_counter = 0xFFFF;
			setIrqSignalOut(true);
		}
	}
}

bool CpuMapper67::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << irq_enable;
	s << irq_toggle;
	s << irq_counter;
	return true;
}

bool CpuMapper67::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> irq_enable;
	s >> irq_toggle;
	s >> irq_counter;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(67, "SunSoft Mapper 3")
