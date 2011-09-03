#include "mapper254.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper254::CpuMapper254(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper254::reset() {
	ppuMapper = mapper()->ppuMapper();

	for (int i = 0; i < 8; i++)
		reg[i] = 0x00;

	protectflag = 0;

	prg0 = 0;
	prg1 = 1;
	setBankCpu();

	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	setBankPpu();

	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0;
	irq_request = 0;
}

quint8 CpuMapper254::readLow(quint16 address) {
	if (address >= 0x6000) {
		if (protectflag)
			return readDirect(address);
		else
			return readDirect(address) ^ 0x01;
	}
	return NesCpuMapper::readLow(address);
}

void CpuMapper254::writeLow(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0x6000:
	case 0x7000:
		writeDirect(address, data);
		break;
	}
}

void CpuMapper254::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		protectflag = 0xFF;
		reg[0] = data;
		setBankCpu();
		setBankPpu();
		break;
	case 0x8001:
		reg[1] = data;

		switch (reg[0] & 0x07) {
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
		break;
	case 0xC000:
		reg[4] = data;
		irq_counter = data;
		irq_request = 0;
		setIrqSignalOut(false);
		break;
	case 0xC001:
		reg[5] = data;
		irq_latch = data;
		irq_request = 0;
		setIrqSignalOut(false);
		break;
	case 0xE000:
		reg[6] = data;
		irq_enable = 0;
		irq_request = 0;
		setIrqSignalOut(false);
		break;
	case 0xE001:
		reg[7] = data;
		irq_enable = 1;
		irq_request = 0;
		setIrqSignalOut(false);
		break;
	}
}

PpuMapper254::PpuMapper254(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper254::reset() {
	cpuMapper = static_cast<CpuMapper254 *>(mapper()->cpuMapper());
}

void PpuMapper254::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && cpuMapper->ppu()->registers()->isDisplayOn()) {
		if (cpuMapper->irq_enable && !cpuMapper->irq_request) {
			if (scanline == 0) {
				if (cpuMapper->irq_counter)
					cpuMapper->irq_counter--;
			}
			if (!(cpuMapper->irq_counter--)) {
				cpuMapper->irq_request = 0xFF;
				cpuMapper->irq_counter = cpuMapper->irq_latch;
				cpuMapper->setIrqSignalOut(true);
			}
		}
	}
}

void CpuMapper254::setBankCpu() {
	if (reg[0] & 0x40)
		setRom8KBanks(romSize8KB()-2, prg1, prg0, romSize8KB()-1);
	else
		setRom8KBanks(prg0, prg1, romSize8KB()-2, romSize8KB()-1);
}

void CpuMapper254::setBankPpu() {
	if (ppuMapper->vromSize1KB()) {
		if (reg[0] & 0x80) {
			ppuMapper->setVrom1KBank(4, (chr01+0));
			ppuMapper->setVrom1KBank(5, (chr01+1));
			ppuMapper->setVrom1KBank(6, (chr23+0));
			ppuMapper->setVrom1KBank(7, (chr23+1));
			ppuMapper->setVrom1KBank(0, chr4);
			ppuMapper->setVrom1KBank(1, chr5);
			ppuMapper->setVrom1KBank(2, chr6);
			ppuMapper->setVrom1KBank(3, chr7);
		} else {
			ppuMapper->setVrom1KBank(0, (chr01+0));
			ppuMapper->setVrom1KBank(1, (chr01+1));
			ppuMapper->setVrom1KBank(2, (chr23+0));
			ppuMapper->setVrom1KBank(3, (chr23+1));
			ppuMapper->setVrom1KBank(4, chr4);
			ppuMapper->setVrom1KBank(5, chr5);
			ppuMapper->setVrom1KBank(6, chr6);
			ppuMapper->setVrom1KBank(7, chr7);
		}
	} else {
		if (reg[0] & 0x80) {
			ppuMapper->setCram1KBank(4, (chr01+0)&0x07);
			ppuMapper->setCram1KBank(5, (chr01+1)&0x07);
			ppuMapper->setCram1KBank(6, (chr23+0)&0x07);
			ppuMapper->setCram1KBank(7, (chr23+1)&0x07);
			ppuMapper->setCram1KBank(0, chr4&0x07);
			ppuMapper->setCram1KBank(1, chr5&0x07);
			ppuMapper->setCram1KBank(2, chr6&0x07);
			ppuMapper->setCram1KBank(3, chr7&0x07);
		} else {
			ppuMapper->setCram1KBank(0, (chr01+0)&0x07);
			ppuMapper->setCram1KBank(1, (chr01+1)&0x07);
			ppuMapper->setCram1KBank(2, (chr23+0)&0x07);
			ppuMapper->setCram1KBank(3, (chr23+1)&0x07);
			ppuMapper->setCram1KBank(4, chr4&0x07);
			ppuMapper->setCram1KBank(5, chr5&0x07);
			ppuMapper->setCram1KBank(6, chr6&0x07);
			ppuMapper->setCram1KBank(7, chr7&0x07);
		}
	}
}

bool CpuMapper254::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 8; i++)
		s << reg[i];
	s << reg[8];
	s << prg0 << prg1;
	s << chr01 << chr23 << chr4 << chr5 << chr6 << chr7;

	s << irq_type;
	s << irq_enable;
	s << irq_counter;
	s << irq_latch;
	s << irq_request;
	s << protectflag;
	return true;
}

bool CpuMapper254::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 8; i++)
		s >> reg[i];
	s >> reg[8];
	s >> prg0 >> prg1;
	s >> chr01 >> chr23 >> chr4 >> chr5 >> chr6 >> chr7;

	s >> irq_type;
	s >> irq_enable;
	s >> irq_counter;
	s >> irq_latch;
	s >> irq_request;
	s >> protectflag;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(254, "Pokemon Pirate Cart")
