#include "mapper45.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

CpuMapper45::CpuMapper45(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper45::reset() {
	ppuMapper = static_cast<PpuMapper45 *>(mapper()->ppuMapper());

	patch = 0;
	qMemSet(reg, 0, sizeof(reg));

	prg0 = 0;
	prg1 = 1;
	prg2 = romSize8KB()-2;
	prg3 = romSize8KB()-1;

	quint32 crc = disk()->crc();
	if (crc == 0x58bcacf6		// Kunio 8-in-1 (Pirate Cart)
	 || crc == 0x9103cfd6		// HIK 7-in-1 (Pirate Cart)
	 || crc == 0xc082e6d3) {	// Super 8-in-1 (Pirate Cart)
		patch = 1;
		prg2 = 62;
		prg3 = 63;
	}
	if (crc == 0xe0dd259d) {	// Super 3-in-1 (Pirate Cart)
		patch = 2;
	}

	setRom8KBanks(prg0, prg1, prg2, prg3);

	p[0] = prg0;
	p[1] = prg1;
	p[2] = prg2;
	p[3] = prg3;

	ppuMapper->setVrom8KBank(0);

	chr0 = c[0] = 0;
	chr1 = c[1] = 1;
	chr2 = c[2] = 2;
	chr3 = c[3] = 3;
	chr4 = c[4] = 4;
	chr5 = c[5] = 5;
	chr6 = c[6] = 6;
	chr7 = c[7] = 7;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_latched = 0;
	irq_reset = 0;
}

void CpuMapper45::writeLow(quint16 address, quint8 data) {
	Q_UNUSED(address)
	if (!(reg[3]&0x40)) {
		reg[reg[5]] = data;
		reg[5] = (reg[5]+1) & 0x03;

		setBankCpu(4, prg0);
		setBankCpu(5, prg1);
		setBankCpu(6, prg2);
		setBankCpu(7, prg3);
		setBankPpu();
	}
}

void CpuMapper45::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		if ((data&0x40)!=(reg[6]&0x40)) {
			quint8 swp;
			swp = prg0; prg0 = prg2; prg2 = swp;
			swp = p[0]; p[0] = p[2]; p[2] = swp;
			setBankCpu(4, p[0]);
			setBankCpu(5, p[1]);
		}
		if (ppuMapper->vromSize1KB()) {
			if ((data&0x80)!=(reg[6]&0x80)) {
				quint32 swp;
				swp = chr4; chr4 = chr0; chr0 = swp;
				swp = chr5; chr5 = chr1; chr1 = swp;
				swp = chr6; chr6 = chr2; chr2 = swp;
				swp = chr7; chr7 = chr3; chr3 = swp;
				swp = c[4]; c[4] = c[0]; c[0] = swp;
				swp = c[5]; c[5] = c[1]; c[1] = swp;
				swp = c[6]; c[6] = c[2]; c[2] = swp;
				swp = c[7]; c[7] = c[3]; c[3] = swp;
				for (int i = 0; i < 8; i++)
					ppuMapper->setVrom1KBank(i, c[i]);
			}
		}
		reg[6] = data;
		break;
	case 0x8001:
		switch( reg[6] & 0x07) {
			case 0x00:
				chr0 = (data & 0xFE)+0;
				chr1 = (data & 0xFE)+1;
				setBankPpu();
				break;
			case 0x01:
				chr2 = (data & 0xFE)+0;
				chr3 = (data & 0xFE)+1;
				setBankPpu();
				break;
			case 0x02:
				chr4 = data;
				setBankPpu();
				break;
			case 0x03:
				chr5 = data;
				setBankPpu();
				break;
			case 0x04:
				chr6 = data;
				setBankPpu();
				break;
			case 0x05:
				chr7 = data;
				setBankPpu();
				break;
			case 0x06:
				if (reg[6] & 0x40) {
					prg2 = data & 0x3F;
					setBankCpu(6, data);
				} else {
					prg0 = data & 0x3F;
					setBankCpu(4, data);
				}
				break;
			case 0x07:
				prg1 = data & 0x3F;
				setBankCpu(5, data);
				break;
		}
		break;
	case 0xA000:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 0x01));
		break;
	case 0xC000:
		if (patch == 2) {
			if (data == 0x29 || data == 0x70)
				data = 0x07;
		}
		irq_latch = data;
		irq_latched = 1;
		if (irq_reset) {
			irq_counter = data;
			irq_latched = 0;
		}
		break;
	case 0xC001:
		irq_counter = irq_latch;
		break;
	case 0xE000:
		irq_enable = 0;
		irq_reset = 1;
		setIrqSignalOut(false);
		break;
	case 0xE001:
		irq_enable = 1;
		if (irq_latched)
			irq_counter = irq_latch;
		break;
	}
}

void CpuMapper45::setBankCpu(uint page, uint bank) {
	Q_ASSERT(bank >= 4 && bank < 8);
	bank &= (reg[3] & 0x3F) ^ 0xFF;
	bank &= 0x3F;
	bank |= reg[1];
	setRom8KBank(page, bank);
	p[bank - 4] = bank;
}

void CpuMapper45::setBankPpu() {
	static quint8 table[16] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF
	};

	c[0] = chr0;
	c[1] = chr1;
	c[2] = chr2;
	c[3] = chr3;
	c[4] = chr4;
	c[5] = chr5;
	c[6] = chr6;
	c[7] = chr7;

	for (int i = 0; i < 8; i++) {
		c[i] &= table[reg[2] & 0x0F];
		c[i] |= reg[0] & ((patch!=1)?0xFF:0xC0);
		c[i] += (reg[2] & ((patch!=1)?0x10:0x30))<<4;
	}

	if (reg[6] & 0x80) {
		for (int i = 0; i < 8; i++)
			ppuMapper->setVrom1KBank(i, c[i ^ 4]);
	} else {
		for (int i = 0; i < 8; i++)
			ppuMapper->setVrom1KBank(i, c[i]);
	}
}

PpuMapper45::PpuMapper45(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper45::reset() {
	cpuMapper = static_cast<CpuMapper45 *>(mapper()->cpuMapper());
}

void PpuMapper45::horizontalSync(int scanline) {
	cpuMapper->irq_reset = 0;
	if (scanline < NesPpu::VisibleScreenHeight && cpuMapper->ppu()->registers()->isDisplayOn()) {
		if (cpuMapper->irq_counter) {
			cpuMapper->irq_counter--;
			if (cpuMapper->irq_counter == 0) {
				if (cpuMapper->irq_enable)
					cpuMapper->setIrqSignalOut(true);
			}
		}
	}
}

bool CpuMapper45::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 8; i++)
		s << reg[i];
	s << prg0 << prg1 << prg2 << prg3;
	s << chr0 << chr1 << chr2 << chr3 << chr4 << chr5 << chr6 << chr7;
	for (int i = 0; i < 4; i++)
		s << p[i];
	for (int i = 0; i < 8; i++)
		s << c[i];
	s << irq_enable;
	s << irq_counter;
	s << irq_latch;
	s << irq_latched;
	s << irq_reset;
	return true;
}

bool CpuMapper45::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 8; i++)
		s >> reg[i];
	s >> prg0 >> prg1 >> prg2 >> prg3;
	s >> chr0 >> chr1 >> chr2 >> chr3 >> chr4 >> chr5 >> chr6 >> chr7;
	for (int i = 0; i < 4; i++)
		s >> p[i];
	for (int i = 0; i < 8; i++)
		s >> c[i];
	s >> irq_enable;
	s >> irq_counter;
	s >> irq_latch;
	s >> irq_latched;
	s >> irq_reset;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(45, "1000000-in-1")
