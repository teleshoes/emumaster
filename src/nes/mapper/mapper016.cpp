#include "mapper016.h"
#include "disk.h"
#include "ppu.h"

void Mapper016::reset() {
	NesMapper::reset();

	patch = 0;

	reg[0] = reg[1] = reg[2] = 0;
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;

	irq_type = IrqClock;

	eeprom_type = 0;

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);

	u32 crc = nesDiskCrc;

	if (crc == 0x3f15d20d		// Famicom Jump 2(J)
	 || crc == 0xf76aa523) {	// Famicom Jump 2(J)(alt)
		patch = 1;
		eeprom_type = 0xFF;

		nesWram[0x0BBC] = 0xFF;
	}

	if (crc == 0x1d6f27f7) {	// Dragon Ball Z 2(Korean Hack)
		irq_type = IrqHSync;
		eeprom_type = 1;
	}
	if (crc == 0x6f7247c8) {	// Dragon Ball Z 3(Korean Hack)
		eeprom_type = 1;
	}

	if (crc == 0x6c6c2feb		// Dragon Ball 3 - Gokuu Den(J)
	 || crc == 0x8edeb257) {	// Dragon Ball 3 - Gokuu Den(J)(Alt)
	}
	if (crc == 0x31cd9903) {	// Dragon Ball Z - Kyoushuu! Saiya Jin(J)
		irq_type = IrqHSync;
	}
	if (crc == 0xe49fc53e		// Dragon Ball Z 2 - Gekishin Freeza!!(J)
	 || crc == 0x1582fee0) {	// Dragon Ball Z 2 - Gekishin Freeza!!(J) [alt]
		irq_type = IrqHSync;
		eeprom_type = 1;
	}
	if (crc == 0x09499f4d) {	// Dragon Ball Z 3 - Ressen Jinzou Ningen(J)
		irq_type = IrqHSync;
		eeprom_type = 1;
	}
	if (crc == 0x2e991109) {	// Dragon Ball Z Gaiden - Saiya Jin Zetsumetsu Keikaku (J)
		irq_type = IrqHSync;
		eeprom_type = 1;
	}

	if (crc == 0x73ac76db		// SD Gundam Gaiden - Knight Gundam Monogatari 2 - Hikari no Kishi(J)
	 || crc == 0x81a15eb8) {	// SD Gundam Gaiden - Knight Gundam Monogatari 3 - Densetsu no Kishi Dan(J)
		eeprom_type = 1;
	}
	if (crc == 0x170250de) {	// Rokudenashi Blues(J)
		nesPpu.setRenderMethod(NesPpu::PreAllRender);
		eeprom_type = 1;
	}

	if (crc == 0x0be0a328 		// Datach - SD Gundam - Gundam Wars(J)
	 || crc == 0x19e81461		// Datach - Dragon Ball Z - Gekitou Tenkaichi Budou Kai(J)
	 || crc == 0x5b457641		// Datach - Ultraman Club - Supokon Fight!(J)
	 || crc == 0x894efdbc		// Datach - Crayon Shin Chan - Ora to Poi Poi(J)
	 || crc == 0x983d8175		// Datach - Battle Rush - Build Up Robot Tournament(J)
	 || crc == 0xbe06853f) {	// Datach - J League Super Top Players(J)
		eeprom_type = 2;
	}
	if (crc == 0xf51a7f46) {	// Datach - Yuu Yuu Hakusho - Bakutou Ankoku Bujutsu Kai(J)
		irq_type = IrqHSync;
		eeprom_type = 2;
	}

	if (eeprom_type == 0) {
		x24c01.reset(nesWram);
	} else if (eeprom_type == 1) {
		x24c02.reset(nesWram);
	} else if (eeprom_type == 2) {
		x24c02.reset(nesWram);
		x24c01.reset(nesWram+256);
	}
	setIrqSignalOut(false);
}

u8 Mapper016::readLow(u16 address) {
	if (patch) {
		return NesMapper::readLow(address);
	} else {
		if ((address & 0x00FF) == 0x0000) {
			bool ret = false;
			if (eeprom_type == 0) {
				ret = x24c01.read();
			} else if (eeprom_type == 1) {
				ret = x24c02.read();
			} else if (eeprom_type == 2) {
				ret = x24c02.read() && x24c01.read();
			}
			return	(ret?0x10:0);// TODO barcode |(nes->GetBarcodeStatus());
		}
	}
	return 0x00;
}

void Mapper016::writeLow(u16 address, u8 data) {
	if (!patch) {
		writeSubA(address, data);
	} else {
		NesMapper::writeLow(address, data);
	}
}

void Mapper016::writeHigh(u16 address, u8 data) {
	if (!patch) {
		writeSubA(address, data);
	} else {
		writeSubB(address, data);
	}
}
void Mapper016::writeSubA(u16 address, u8 data) {
	switch(address & 0x000F) {
	case 0x0000:
	case 0x0001:
	case 0x0002:
	case 0x0003:
	case 0x0004:
	case 0x0005:
	case 0x0006:
	case 0x0007:
		if (nesVromSize1KB)
			setVrom1KBank(address&0x0007, data);
		if (eeprom_type == 2) {
			reg[0] = data;
			x24c01.write(data&0x08, reg[1]&0x40);
		}
		break;

	case 0x0008:
		setRom16KBank(4, data);
		break;

	case 0x0009:
		setMirroring(static_cast<NesMirroring>(data & 3));
		break;

	case 0x000A:
		irq_enable = data & 0x01;
		irq_counter = irq_latch;
		setIrqSignalOut(false);
		break;
	case 0x000B:
		irq_latch = (irq_latch & 0xFF00) | data;
		irq_counter = (irq_counter & 0xFF00) | data;
		break;
	case 0x000C:
		irq_latch = (data << 8) | (irq_latch & 0x00FF);
		irq_counter = (data << 8) | (irq_counter & 0x00FF);
		break;

	case 0x000D:
		// EEPTYPE0(DragonBallZ)
		if (eeprom_type == 0) {
			x24c01.write(data&0x20, data&0x40);
		}
		// EEPTYPE1(DragonBallZ2,Z3,Z Gaiden)
		if (eeprom_type == 1) {
			x24c02.write(data&0x20, data&0x40);
		}
		// EEPTYPE2(DATACH)
		if (eeprom_type == 2) {
			reg[1] = data;
			x24c02.write(data&0x20, data&0x40);
			x24c01.write(reg[0]&0x08, data&0x40);
		}
		break;
	}
}

// Famicom Jump 2
void Mapper016::writeSubB(u16 addr, u8 data)
{
	switch(addr) {
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		reg[0] = data & 0x01;
		setRom8KBank(4, reg[0]*0x20+reg[2]*2+0);
		setRom8KBank(5, reg[0]*0x20+reg[2]*2+1);
		break;
	case 0x8004:
	case 0x8005:
	case 0x8006:
	case 0x8007:
		reg[1] = data & 0x01;
		setRom8KBank(6, reg[1]*0x20+0x1E);
		setRom8KBank(7, reg[1]*0x20+0x1F);
		break;
	case 0x8008:
		reg[2] = data;
		setRom8KBank(4, reg[0]*0x20+reg[2]*2+0);
		setRom8KBank(5, reg[0]*0x20+reg[2]*2+1);
		setRom8KBank(6, reg[1]*0x20+0x1E);
		setRom8KBank(7, reg[1]*0x20+0x1F);
		break;

	case 0x8009:
		setMirroring(static_cast<NesMirroring>(data & 3));
		break;

	case 0x800A:
		irq_enable = data & 0x01;
		irq_counter = irq_latch;
		setIrqSignalOut(false);
		break;
	case 0x800B:
		irq_latch = (irq_latch & 0xFF00) | data;
		break;
	case 0x800C:
		irq_latch = (data << 8) | (irq_latch & 0x00FF);
		break;

	case 0x800D:
		break;
	}
}

void Mapper016::clock(uint cycles) {
	if (irq_enable && irq_type == IrqClock) {
		if ((irq_counter-=cycles) <= 0) {
			setIrqSignalOut(true);
			irq_counter &= 0xFFFF;
		}
	}
}

void Mapper016::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if (irq_enable && irq_type == IrqHSync) {
		if (irq_counter <= 113) {
			setIrqSignalOut(true);
			irq_counter &= 0xFFFF;
		} else {
			irq_counter -= 113;
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper016, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_SUBCALL_##sl(x24c01) \
	STATE_SERIALIZE_SUBCALL_##sl(x24c02) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
STATE_SERIALIZE_END_##sl(Mapper016)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
