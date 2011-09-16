#include "mapper019.h"
#include "ppu.h"
#include "disk.h"
#include <QDataStream>

void Mapper019::reset() {
	NesMapper::reset();

	patch = 0;

	reg[0] = reg[1] = reg[2] = 0;

	qMemSet(exram, 0, sizeof(exram));

	irq_enable = 0;
	irq_counter = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	if (vromSize1KB())
		setVrom8KBank(vromSize8KB()-1);

	exsound_enable = 0xFF;

	quint32 crc = disk()->crc();

	if (crc == 0xb62a7b71) {	// Family Circuit '91(J)
		patch = 1;
	}

	if (crc == 0x02738c68) {	// Wagan Land 2(J)
		patch = 3;
	}
	if (crc == 0x14942c06) {	// Wagan Land 3(J)
		patch = 2;
	}

	if (crc == 0x968dcf09) {	// Final Lap(J)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
	}
	if (crc == 0x3deac303) {	// Rolling Thunder(J)
		ppu()->setRenderMethod(NesPpu::PostAllRender);
	}

	if (crc == 0xb1b9e187) {	// For Kaijuu Monogatari(J)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
	}

	if (crc == 0x6901346e) {	// For Sangokushi 2 - Haou no Tairiku(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}

	if (crc == 0xaf15338f		// For Mindseeker(J)
	 || crc == 0xb1b9e187		// For Kaijuu Monogatari(J)
	 || crc == 0x96533999		// Dokuganryuu Masamune(J)
	 || crc == 0x3296ff7a		// Battle Fleet(J)
	 || crc == 0xdd454208) {	// Hydlide 3(J)
		exsound_enable = 0;
	}

	if (crc == 0x429fd177) {	// Famista '90(J)
		exsound_enable = 0;
	}

	if (exsound_enable) {
		// TODO exsound nes->apu->SelectExSound( 0x10);
	}
}

quint8 Mapper019::readLow(quint16 address) {
	quint8 data = 0;
	switch (address & 0xF800) {
	case 0x4800:
		if (address == 0x4800) {
			if (exsound_enable) {
				// TODO exsound nes->apu->ExRead(address);
				data = exram[reg[2]&0x7F];
			} else {
				data = m_wram[reg[2]&0x7F];
			}
			if (reg[2]&0x80)
				reg[2] = (reg[2]+1)|0x80;
		}
		break;
	case 0x5000:
		data = irq_counter & 0x00FF;
		break;
	case 0x5800:
		data = (irq_counter>>8) & 0x7F;
		break;
	case 0x6000:
	case 0x6800:
	case 0x7000:
	case 0x7800:
		data = NesMapper::readLow(address);
		break;
	default:
		data = address >> 8;
		break;
	}
	return data;
}

void Mapper019::writeLow(quint16 address, quint8 data) {
	switch (address & 0xF800) {
	case 0x4800:
		if (address == 0x4800) {
			if (exsound_enable) {
				// TODO exsound nes->apu->ExWrite(address, data);
				exram[reg[2]&0x7F] = data;
			} else {
				m_wram[reg[2]&0x7F] = data;
			}
			if (reg[2]&0x80)
				reg[2] = (reg[2]+1)|0x80;
		}
		break;
	case 0x5000:
		irq_counter = (irq_counter & 0xFF00) | data;
		setIrqSignalOut(false);
		break;
	case 0x5800:
		irq_counter = (irq_counter & 0x00FF) | ((data & 0x7F) << 8);
		irq_enable  = data & 0x80;
		setIrqSignalOut(false);
		break;
	case 0x6000:
	case 0x6800:
	case 0x7000:
	case 0x7800:
		NesMapper::writeLow(address, data);
		break;
	default:
		break;
	}
}

void Mapper019::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF800) {
	case 0x8000:
		if ((data < 0xE0) || (reg[0] != 0)) {
			setVrom1KBank(0, data);
		} else {
			setCram1KBank(0, data&0x1F);
		}
		break;
	case 0x8800:
		if ((data < 0xE0) || (reg[0] != 0)) {
			setVrom1KBank(1, data);
		} else {
			setCram1KBank(1, data&0x1F);
		}
		break;
	case 0x9000:
		if ((data < 0xE0) || (reg[0] != 0)) {
			setVrom1KBank(2, data);
		} else {
			setCram1KBank(2, data&0x1F);
		}
		break;
	case 0x9800:
		if ((data < 0xE0) || (reg[0] != 0)) {
			setVrom1KBank(3, data);
		} else {
			setCram1KBank(3, data&0x1F);
		}
		break;
	case 0xA000:
		if ((data < 0xE0) || (reg[1] != 0)) {
			setVrom1KBank(4, data);
		} else {
			setCram1KBank(4, data&0x1F);
		}
		break;
	case 0xA800:
		if ((data < 0xE0) || (reg[1] != 0)) {
			setVrom1KBank(5, data);
		} else {
			setCram1KBank(5, data&0x1F);
		}
		break;
	case 0xB000:
		if ((data < 0xE0) || (reg[1] != 0)) {
			setVrom1KBank(6, data);
		} else {
			setCram1KBank(6, data&0x1F);
		}
		break;
	case 0xB800:
		if ((data < 0xE0) || (reg[1] != 0)) {
			setVrom1KBank(7, data);
		} else {
			setCram1KBank(7, data&0x1F);
		}
		break;
	case 0xC000:
		if (!patch) {
			if (data <= 0xDF) {
				setVrom1KBank(8, data);
			} else {
				setVram1KBank(8, data & 0x01);
			}
		}
		break;
	case 0xC800:
		if (!patch) {
			if (data <= 0xDF) {
				setVrom1KBank(9, data);
			} else {
				setVram1KBank(9, data & 0x01);
			}
		}
		break;
	case 0xD000:
		if (!patch) {
			if (data <= 0xDF) {
				setVrom1KBank(10, data);
			} else {
				setVram1KBank(10, data & 0x01);
			}
		}
		break;
	case 0xD800:
		if (!patch) {
			if (data <= 0xDF) {
				setVrom1KBank(11, data);
			} else {
				setVram1KBank(11, data & 0x01);
			}
		}
		break;
	case 0xE000:
		setRom8KBank(4, data & 0x3F);
		if (patch == 2) {
			if (data & 0x40)
				setMirroring(Vertical);
			else
				setMirroring(SingleLow);
		}
		if (patch == 3) {
			if (data & 0x80)
				setMirroring(Horizontal);
			else
				setMirroring(Vertical);
		}
		break;
	case 0xE800:
		reg[0] = data & 0x40;
		reg[1] = data & 0x80;
		setRom8KBank(5, data & 0x3F);
		break;
	case 0xF000:
		setRom8KBank(6, data & 0x3F);
		break;
	case 0xF800:
		if (address == 0xF800) {
			if (exsound_enable) {
				// TODO exsound nes->apu->ExWrite( address, data);
			}
			reg[2] = data;
		}
		break;
	default:
		break;
	}
}

void Mapper019::clock(uint cycles) {
	if (irq_enable) {
		if ((irq_counter+=cycles) >= 0x7FFF) {
			irq_enable  = 0;
			irq_counter = 0x7FFF;
			setIrqSignalOut(true);
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper019, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_ARRAY_##sl(exram, sizeof(exram)) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
STATE_SERIALIZE_END_##sl(Mapper019)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
