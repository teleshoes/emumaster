#include "mapper069.h"
#include "disk.h"
#include "ppu.h"
#include <QDataStream>

void Mapper069::reset() {
	NesMapper::reset();

	patch = 0;

	// TODO nes->apu->SelectExSound(32);

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	if (vromSize1KB())
		setVrom8KBank(0);

	irq_enable = 0;
	irq_counter = 0;


	quint32 crc = disk()->crc();

	if (crc == 0xfeac6916)	// Honoo no Toukyuuji - Dodge Danpei 2(J)
		ppu()->setRenderMethod(NesPpu::TileRender);

	if (crc == 0xad28aef6)	// Dynamite Batman(J) / Dynamite Batman - Return of the Joker(U)
		patch = 1;
}

void Mapper069::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE000) {
	case 0x8000:
		reg = data;
		break;

	case 0xA000:
		switch( reg & 0x0F) {
		case 0x00:	case 0x01:
		case 0x02:	case 0x03:
		case 0x04:	case 0x05:
		case 0x06:	case 0x07:
			setVrom1KBank(reg&0x07, data);
			break;
		case 0x08:
			if (!patch && !(data & 0x40)) {
				setRom8KBank(3, data);
			}
			break;
		case 0x09:
			setRom8KBank(4, data);
			break;
		case 0x0A:
			setRom8KBank(5, data);
			break;
		case 0x0B:
			setRom8KBank(6, data);
			break;

		case 0x0C:
			setMirroring(static_cast<Mirroring>(data & 0x03));
			break;

		case 0x0D:
			irq_enable = data;
			setIrqSignalOut(false);
			break;

		case 0x0E:
			irq_counter = (irq_counter & 0xFF00) | data;
			setIrqSignalOut(false);
			break;

		case 0x0F:
			irq_counter = (irq_counter & 0x00FF) | (data << 8);
			setIrqSignalOut(false);
			break;
		}
		break;

	case 0xC000:
	case 0xE000:
		// TODO nes->apu->ExWrite( address, data);
		break;
	}
}

void Mapper069::clock(uint cycles) {
	if (irq_enable) {
		irq_counter -= cycles;
		if (irq_counter <= 0) {
			setIrqSignalOut(true);
			irq_enable = 0;
			irq_counter = 0xFFFF;
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper069, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(reg) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
STATE_SERIALIZE_END_##sl(Mapper069)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
