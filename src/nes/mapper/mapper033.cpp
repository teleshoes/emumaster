#include "mapper033.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

void Mapper033::reset() {
	NesMapper::reset();

	reg[0] = 0;
	reg[1] = 2;
	reg[2] = 4;
	reg[3] = 5;
	reg[4] = 6;
	reg[5] = 7;
	reg[6] = 1;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (vromSize8KB())
		updateBanks();

	quint32 crc = disk()->crc();
	// Check For Old #33 games.... (CRC code by NesToy)
	if (crc == 0x5e9bc161		// Akira(J)
	 || crc == 0xecdbafa4		// Bakushou!! Jinsei Gekijou(J)
	 || crc == 0x59cd0c31		// Don Doko Don(J)
	 || crc == 0x837c1342		// Golf Ko Open(J)
	 || crc == 0x42d893e4		// Operation Wolf(J)
	 || crc == 0x1388aeb9		// Operation Wolf(U)
	 || crc == 0x07ee6d8f		// Power Blazer(J)
	 || crc == 0x5193fb54		// Takeshi no Sengoku Fuuunji(J)
	 || crc == 0xa71c3452) {	// Insector X(J)
		patch = 1;
	}

	ppu()->setRenderMethod(NesPpu::PreRender);

	if (crc == 0x202df297) {	// Captain Saver(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x63bb86b5) {	// The Jetsons(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
}

void Mapper033::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		if (patch) {
			if (data & 0x40)
				setMirroring(Horizontal);
			else
				setMirroring(Vertical);
			setRom8KBank(4, data & 0x1F);
		} else {
			setRom8KBank(4, data);
		}
		break;
	case 0x8001:
		if (patch) {
			setRom8KBank(5, data & 0x1F);
		} else {
			setRom8KBank(5, data);
		}
		break;

	case 0x8002:
		reg[0] = data;
		updateBanks();
		break;
	case 0x8003:
		reg[1] = data;
		updateBanks();
		break;
	case 0xA000:
		reg[2] = data;
		updateBanks();
		break;
	case 0xA001:
		reg[3] = data;
		updateBanks();
		break;
	case 0xA002:
		reg[4] = data;
		updateBanks();
		break;
	case 0xA003:
		reg[5] = data;
		updateBanks();
		break;

	case 0xC000:
		irq_latch = data;
		irq_counter = irq_latch;
		break;
	case 0xC001:
		irq_counter = irq_latch;
		break;
	case 0xC002:
		irq_enable = 1;
		break;
	case 0xC003:
		irq_enable = 0;
		break;

	case 0xE001:
	case 0xE002:
	case 0xE003:
		break;
	case 0xE000:
		if (data & 0x40)
			setMirroring(Horizontal);
		else
			setMirroring(Vertical);
		break;
	}
}

void Mapper033::updateBanks() {
	setVrom2KBank(0, reg[0]);
	setVrom2KBank(2, reg[1]);

	setVrom1KBank(4, reg[2]);
	setVrom1KBank(5, reg[3]);
	setVrom1KBank(6, reg[4]);
	setVrom1KBank(7, reg[5]);
}

void Mapper033::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && ppuRegisters()->isDisplayOn()) {
		if (irq_enable) {
			if (++irq_counter == 0) {
				irq_enable  = 0;
				irq_counter = 0;
				setIrqSignalOut(true);
			}
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
	STATE_SERIALIZE_BEGIN_##sl(Mapper033) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
	STATE_SERIALIZE_END(Mapper033)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)