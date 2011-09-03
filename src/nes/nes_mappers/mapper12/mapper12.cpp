#include "mapper12.h"
#include "nesppu.h"
#include "nesdisk.h"

class NesMapper12Data {
public:
	quint32 vb0, vb1;
	quint8	reg[8];
	quint8	prg0, prg1;
	quint8	chr01, chr23, chr4, chr5, chr6, chr7;

	quint8	irq_enable;
	quint8	irq_counter;
	quint8	irq_latch;
	quint8	irq_request;
	quint8	irq_preset;
	quint8	irq_preset_vbl;
};

CpuMapper12::CpuMapper12(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
	d = new NesMapper12Data();
}

CpuMapper12::~CpuMapper12() {
	delete d;
}

void CpuMapper12::reset() {
	ppuMapper = static_cast<PpuMapper12 *>(mapper()->ppuMapper());
	qMemSet(d->reg, 0, sizeof(d->reg));

	d->prg0 = 0;
	d->prg1 = 1;
	updateBanks();

	d->irq_enable = 0;	// Disable
	d->irq_counter = 0;
	d->irq_latch = 0xFF;
	d->irq_request = 0;
	d->irq_preset = 0;
	d->irq_preset_vbl = 0;
}

quint8 CpuMapper12::readLow(quint16 address) {
	Q_UNUSED(address)
	return 0x01;
}

void CpuMapper12::writeLow(quint16 address, quint8 data) {
	if (address > 0x4100 && address < 0x6000) {
		d->vb0 = (data&0x01)<<8;
		d->vb1 = (data&0x10)<<4;
		ppuMapper->updateBanks();
	} else {
		CpuMapper12::writeLow(address, data);
	}
}

void CpuMapper12::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		d->reg[0] = data;
		updateBanks();
		ppuMapper->updateBanks();
		break;
	case 0x8001:
		d->reg[1] = data;
		switch (d->reg[0] & 0x07) {
		case 0x00: d->chr01 = data & 0xFE; ppuMapper->updateBanks(); break;
		case 0x01: d->chr23 = data & 0xFE; ppuMapper->updateBanks(); break;
		case 0x02: d->chr4 = data; ppuMapper->updateBanks(); break;
		case 0x03: d->chr5 = data; ppuMapper->updateBanks(); break;
		case 0x04: d->chr6 = data; ppuMapper->updateBanks(); break;
		case 0x05: d->chr7 = data; ppuMapper->updateBanks(); break;
		case 0x06: d->prg0 = data; updateBanks(); break;
		case 0x07: d->prg1 = data; updateBanks(); break;
		}
		break;
	case 0xA000:
		d->reg[2] = data;
		if (disk()->mirroring() != NesPpuMapper::FourScreen) {
			if (data & 0x01)
				ppuMapper->setMirroring(NesPpuMapper::Horizontal);
			else
				ppuMapper->setMirroring(NesPpuMapper::Vertical);
		}
		break;
	case 0xA001:
		d->reg[3] = data;
		break;
	case 0xC000:
		d->reg[4] = data;
		d->irq_latch = data;
		break;
	case 0xC001:
		d->reg[5] = data;
		if (ppu()->scanline() < NesPpu::VisibleScreenHeight) {
			d->irq_counter |= 0x80;
			d->irq_preset = 0xFF;
		} else {
			d->irq_counter |= 0x80;
			d->irq_preset_vbl = 0xFF;
			d->irq_preset = 0;
		}
		break;
	case 0xE000:
		d->reg[6] = data;
		d->irq_enable = 0;
		d->irq_request = 0;
		setIrqSignalOut(false);
		break;
	case 0xE001:
		d->reg[7] = data;
		d->irq_enable = 1;
		d->irq_request = 0;
		break;
	}
}

void CpuMapper12::updateBanks() {
	if (d->reg[0] & 0x40)
		setRom8KBanks(romSize8KB()-2, d->prg1, d->prg0, romSize8KB()-1);
	else
		setRom8KBanks(d->prg0, d->prg1, romSize8KB()-2, romSize8KB()-1);
}

PpuMapper12::PpuMapper12(NesMapper *mapper) :
	NesPpuMapper(mapper),
	d(0),
	cpuMapper(0),
	ppuRegisters(0) {
}

void PpuMapper12::reset() {
	cpuMapper = static_cast<CpuMapper12 *>(mapper()->cpuMapper());
	d = cpuMapper->d;
	ppuRegisters = cpuMapper->ppu()->registers();

	d->vb0 = 0;
	d->vb1 = 0;
	d->chr01 = 0;
	d->chr23 = 2;
	d->chr4  = 4;
	d->chr5  = 5;
	d->chr6  = 6;
	d->chr7  = 7;
	updateBanks();
}

void PpuMapper12::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && ppuRegisters->isDisplayOn()) {
		if (d->irq_preset_vbl) {
			d->irq_counter = d->irq_latch;
			d->irq_preset_vbl = 0;
		}
		if (d->irq_preset) {
			d->irq_counter = d->irq_latch;
			d->irq_preset = 0;
		} else if (d->irq_counter > 0) {
			d->irq_counter--;
		}
		if (d->irq_counter == 0) {
			if (d->irq_enable) {
				d->irq_request = 0xFF;
				cpuMapper->setIrqSignalOut(true);
			}
			d->irq_preset = 0xFF;
		}
	}
}

void PpuMapper12::updateBanks() {
	if (vromSize1KB()) {
		if (d->reg[0] & 0x80) {
			setVrom1KBank(4, d->vb1 + d->chr01);
			setVrom1KBank(5, d->vb1 + d->chr01+1);
			setVrom1KBank(6, d->vb1 + d->chr23);
			setVrom1KBank(7, d->vb1 + d->chr23+1);
			setVrom1KBank(0, d->vb0 + d->chr4);
			setVrom1KBank(1, d->vb0 + d->chr5);
			setVrom1KBank(2, d->vb0 + d->chr6);
			setVrom1KBank(3, d->vb0 + d->chr7);
		} else {
			setVrom1KBank(0, d->vb0 + d->chr01);
			setVrom1KBank(1, d->vb0 + d->chr01+1);
			setVrom1KBank(2, d->vb0 + d->chr23);
			setVrom1KBank(3, d->vb0 + d->chr23+1);
			setVrom1KBank(4, d->vb1 + d->chr4);
			setVrom1KBank(5, d->vb1 + d->chr5);
			setVrom1KBank(6, d->vb1 + d->chr6);
			setVrom1KBank(7, d->vb1 + d->chr7);
		}
	} else {
		if (d->reg[0] & 0x80) {
			setCram1KBank(4, (d->chr01+0)&0x07);
			setCram1KBank(5, (d->chr01+1)&0x07);
			setCram1KBank(6, (d->chr23+0)&0x07);
			setCram1KBank(7, (d->chr23+1)&0x07);
			setCram1KBank(0, d->chr4&0x07);
			setCram1KBank(1, d->chr5&0x07);
			setCram1KBank(2, d->chr6&0x07);
			setCram1KBank(3, d->chr7&0x07);
		} else {
			setCram1KBank(0, (d->chr01+0)&0x07);
			setCram1KBank(1, (d->chr01+1)&0x07);
			setCram1KBank(2, (d->chr23+0)&0x07);
			setCram1KBank(3, (d->chr23+1)&0x07);
			setCram1KBank(4, d->chr4&0x07);
			setCram1KBank(5, d->chr5&0x07);
			setCram1KBank(6, d->chr6&0x07);
			setCram1KBank(7, d->chr7&0x07);
		}
	}
}

bool CpuMapper12::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (uint i = 0; i < sizeof(d->reg); i++)
		s << d->reg[i];
	s << d->prg0 << d->prg1;
	s << d->chr01 << d->chr23 << d->chr4 << d->chr5 << d->chr6 << d->chr7;
	s << d->irq_enable;
	s << d->irq_counter;
	s << d->irq_latch;
	s << d->irq_request;
	s << d->irq_preset;
	s << d->irq_preset_vbl;
	s << d->vb0 << d->vb1;
	return true;
}

bool CpuMapper12::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (uint i = 0; i < sizeof(d->reg); i++)
		s >> d->reg[i];
	s >> d->prg0 >> d->prg1;
	s >> d->chr01 >> d->chr23 >> d->chr4 >> d->chr5 >> d->chr6 >> d->chr7;
	s >> d->irq_enable;
	s >> d->irq_counter;
	s >> d->irq_latch;
	s >> d->irq_request;
	s >> d->irq_preset;
	s >> d->irq_preset_vbl;
	s >> d->vb0 >> d->vb1;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(12, "DBZ5")
