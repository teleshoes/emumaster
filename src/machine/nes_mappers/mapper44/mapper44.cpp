#include "mapper44.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

CpuMapper44::CpuMapper44(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper44::reset() {
	ppuMapper = static_cast<PpuMapper44 *>(mapper()->ppuMapper());

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
	if (ppuMapper->vromSize1KB()) {
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

void CpuMapper44::writeLow(quint16 address, quint8 data) {
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

void CpuMapper44::writeHigh(quint16 address, quint8 data) {
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
		if (disk()->mirroring() != NesPpuMapper::FourScreen)
			ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 0x01));
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

void CpuMapper44::setBankCpu() {
	if (reg[0] & 0x40) {
		setRom8KBank(4, ((bank == 6)?0x1e:0x0e)|(bank<<4));
		setRom8KBank(5, ((bank == 6)?0x1f&prg1:0x0f&prg1)|(bank<<4));
		setRom8KBank(6, ((bank == 6)?0x1f&prg0:0x0f&prg0)|(bank<<4));
		setRom8KBank(7, ((bank == 6)?0x1f:0x0f)|(bank<<4));
	} else {
		setRom8KBank(4, ((bank == 6)?0x1f&prg0:0x0f&prg0)|(bank<<4));
		setRom8KBank(5, ((bank == 6)?0x1f&prg1:0x0f&prg1)|(bank<<4));
		setRom8KBank(6, ((bank == 6)?0x1e:0x0e)|(bank<<4));
		setRom8KBank(7, ((bank == 6)?0x1f:0x0f)|(bank<<4));
	}
}

void CpuMapper44::setBankPpu() {
	if (ppuMapper->vromSize1KB()) {
		if (reg[0] & 0x80) {
			ppuMapper->setVrom1KBank(0, ((bank == 6)?0xff&chr4:0x7f&chr4)|(bank<<7));
			ppuMapper->setVrom1KBank(1, ((bank == 6)?0xff&chr5:0x7f&chr5)|(bank<<7));
			ppuMapper->setVrom1KBank(2, ((bank == 6)?0xff&chr6:0x7f&chr6)|(bank<<7));
			ppuMapper->setVrom1KBank(3, ((bank == 6)?0xff&chr7:0x7f&chr7)|(bank<<7));
			ppuMapper->setVrom1KBank(4, ((bank == 6)?0xff&chr01:0x7f&chr01)|(bank<<7));
			ppuMapper->setVrom1KBank(5, ((bank == 6)?0xff&(chr01+1):0x7f&(chr01+1))|(bank<<7));
			ppuMapper->setVrom1KBank(6, ((bank == 6)?0xff&chr23:0x7f&chr23)|(bank<<7));
			ppuMapper->setVrom1KBank(7, ((bank == 6)?0xff&(chr23+1):0x7f&(chr23+1))|(bank<<7));
		} else {
			ppuMapper->setVrom1KBank(0, ((bank == 6)?0xff&chr01:0x7f&chr01)|(bank<<7));
			ppuMapper->setVrom1KBank(1, ((bank == 6)?0xff&(chr01+1):0x7f&(chr01+1))|(bank<<7));
			ppuMapper->setVrom1KBank(2, ((bank == 6)?0xff&chr23:0x7f&chr23)|(bank<<7));
			ppuMapper->setVrom1KBank(3, ((bank == 6)?0xff&(chr23+1):0x7f&(chr23+1))|(bank<<7));
			ppuMapper->setVrom1KBank(4, ((bank == 6)?0xff&chr4:0x7f&chr4)|(bank<<7));
			ppuMapper->setVrom1KBank(5, ((bank == 6)?0xff&chr5:0x7f&chr5)|(bank<<7));
			ppuMapper->setVrom1KBank(6, ((bank == 6)?0xff&chr6:0x7f&chr6)|(bank<<7));
			ppuMapper->setVrom1KBank(7, ((bank == 6)?0xff&chr7:0x7f&chr7)|(bank<<7));
		}
	}
}

PpuMapper44::PpuMapper44(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper44::reset() {
	cpuMapper = static_cast<CpuMapper44 *>(mapper()->cpuMapper());
}

void PpuMapper44::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && cpuMapper->ppu()->registers()->isDisplayOn()) {
		if (cpuMapper->irq_enable) {
			if (!(--cpuMapper->irq_counter)) {
				cpuMapper->irq_counter = cpuMapper->irq_latch;
				cpuMapper->setIrqSignalOut(true);
			}
		}
	}
}

bool CpuMapper44::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 8; i++)
		s << reg[i];
	s << bank;
	s << prg0 << prg1;
	s << chr01 << chr23 << chr4 << chr5 << chr6 << chr7;
	s << irq_enable;
	s << irq_counter;
	s << irq_latch;
	return true;
}

bool CpuMapper44::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 8; i++)
		s >> reg[i];
	s >> bank;
	s >> prg0 >> prg1;
	s >> chr01 >> chr23 >> chr4 >> chr5 >> chr6 >> chr7;
	s >> irq_enable;
	s >> irq_counter;
	s >> irq_latch;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(44, "Super HiK 7-in-1")
