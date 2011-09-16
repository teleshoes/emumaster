#include "mapper047.h"
#include "disk.h"
#include "ppu.h"
#include <QDataStream>

void Mapper047::reset() {
	NesMapper::reset();

	patch = 0;

	quint32 crc = disk()->crc();
	if (crc == 0x7eef434c) {
		patch = 1;
	}

	qMemSet(reg, 0, sizeof(reg));

	bank = 0;
	prg0 = 0;
	prg1 = 1;

	// set VROM banks
	if (vromSize1KB()) {
		chr01 = 0;
		chr23 = 2;
		chr4  = 4;
		chr5  = 5;
		chr6  = 6;
		chr7  = 7;
	} else {
		chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
	}

	setBankCpu();
	setBankPpu();

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
}

void Mapper047::writeLow(quint16 address, quint8 data) {
	if (address == 0x6000) {
		if (patch) {
			bank = (data & 0x06) >> 1;
		} else {
			bank = (data & 0x01) << 1;
		}
		setBankCpu();
		setBankPpu();
	}
}

void Mapper047::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		reg[0] = data;
		setBankCpu();
		setBankPpu();
		break;
	case 0x8001:
		reg[1] = data;
		switch( reg[0] & 0x07) {
			case 0x00: chr01 = data & 0xFE; setBankPpu(); break;
			case 0x01: chr23 = data & 0xFE; setBankPpu(); break;
			case 0x02: chr4 = data; setBankPpu(); break;
			case 0x03: chr5 = data; setBankPpu(); break;
			case 0x04: chr6 = data; setBankPpu(); break;
			case 0x05: chr7 = data; setBankPpu(); break;
			case 0x06: prg0 = data; setBankCpu(); break;
			case 0x07: prg1 = data; setBankCpu(); break;
		}
		break;
	case 0xA000:
		reg[2] = data;
		if (disk()->mirroring() != FourScreen)
			setMirroring(static_cast<Mirroring>(data & 0x01));
		break;
	case 0xA001:
		reg[3] = data;
		bank = data & 0x07;
		if (bank == 7) {
			bank = 6;
		}
		setBankCpu();
		setBankPpu();
		break;
	case 0xC000:
		reg[4] = data;
		irq_counter = data;
		break;
	case 0xC001:
		reg[5] = data;
		irq_latch = data;
		break;
	case 0xE000:
		reg[6] = data;
		irq_enable = 0;
		setIrqSignalOut(false);
		break;
	case 0xE001:
		reg[7] = data;
		irq_enable = 1;
		break;
	}
}

void Mapper047::setBankCpu() {
	if (reg[0] & 0x40) {
		setRom8KBank(4, bank * 8 + ((patch && bank != 2)?6:14));
		setRom8KBank(5, bank * 8 + prg1);
		setRom8KBank(6, bank * 8 + prg0);
		setRom8KBank(7, bank * 8 + ((patch && bank != 2)?7:15));
	} else {
		setRom8KBank(4, bank * 8 + prg0);
		setRom8KBank(5, bank * 8 + prg1);
		setRom8KBank(6, bank * 8 + ((patch && bank != 2)?6:14));
		setRom8KBank(7, bank * 8 + ((patch && bank != 2)?7:15));
	}
}

void Mapper047::setBankPpu() {
	if (vromSize1KB()) {
		if (reg[0] & 0x80) {
			setVrom1KBank(0, (bank & 0x02) * 64 + chr4);
			setVrom1KBank(1, (bank & 0x02) * 64 + chr5);
			setVrom1KBank(2, (bank & 0x02) * 64 + chr6);
			setVrom1KBank(3, (bank & 0x02) * 64 + chr7);
			setVrom1KBank(4, (bank & 0x02) * 64 + chr01 + 0);
			setVrom1KBank(5, (bank & 0x02) * 64 + chr01 + 1);
			setVrom1KBank(6, (bank & 0x02) * 64 + chr23 + 0);
			setVrom1KBank(7, (bank & 0x02) * 64 + chr23 + 1);
		} else {
			setVrom1KBank(0, (bank & 0x02) * 64 + chr01 + 0);
			setVrom1KBank(1, (bank & 0x02) * 64 + chr01 + 1);
			setVrom1KBank(2, (bank & 0x02) * 64 + chr23 + 0);
			setVrom1KBank(3, (bank & 0x02) * 64 + chr23 + 1);
			setVrom1KBank(4, (bank & 0x02) * 64 + chr4);
			setVrom1KBank(5, (bank & 0x02) * 64 + chr5);
			setVrom1KBank(6, (bank & 0x02) * 64 + chr6);
			setVrom1KBank(7, (bank & 0x02) * 64 + chr7);
		}
	}
}

void Mapper047::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && ppuRegisters()->isDisplayOn()) {
		if (irq_enable) {
			if (!(--irq_counter)) {
				irq_counter = irq_latch;
				setIrqSignalOut(true);
			}
		}
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper047, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_ARRAY_##sl(reg, sizeof(reg)) \
	STATE_SERIALIZE_VAR_##sl(bank) \
	STATE_SERIALIZE_VAR_##sl(prg0) \
	STATE_SERIALIZE_VAR_##sl(prg1) \
	STATE_SERIALIZE_VAR_##sl(chr01) \
	STATE_SERIALIZE_VAR_##sl(chr23) \
	STATE_SERIALIZE_VAR_##sl(chr4) \
	STATE_SERIALIZE_VAR_##sl(chr5) \
	STATE_SERIALIZE_VAR_##sl(chr6) \
	STATE_SERIALIZE_VAR_##sl(chr7) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_counter) \
	STATE_SERIALIZE_VAR_##sl(irq_latch) \
STATE_SERIALIZE_END_##sl(Mapper047)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
