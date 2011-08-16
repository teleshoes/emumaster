#include "mapper10.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

class NesMapper10Data {
public:
	quint8 reg[4];
	quint8 latch_a, latch_b;
};

CpuMapper10::CpuMapper10(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
	d = new NesMapper10Data();
}

CpuMapper10::~CpuMapper10() {
	delete d;
}

void CpuMapper10::reset() {
	ppuMapper = static_cast<PpuMapper10 *>(mapper()->ppuMapper());

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	d->latch_a = 0xFE;
	d->latch_b = 0xFE;

	ppu()->setCharacterLatchEnabled(true);
}

void CpuMapper10::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0xA000:
		setRom8KBank(4, data);
		break;
	case 0xB000:
		d->reg[0] = data;
		if (d->latch_a == 0xFD) {
			ppuMapper->setVrom4KBank(0, d->reg[0]);
		}
		break;
	case 0xC000:
		d->reg[1] = data;
		if (d->latch_a == 0xFE) {
			ppuMapper->setVrom4KBank(0, d->reg[1]);
		}
		break;
	case 0xD000:
		d->reg[2] = data;
		if (d->latch_b == 0xFD) {
			ppuMapper->setVrom4KBank(4, d->reg[2]);
		}
		break;
	case 0xE000:
		d->reg[3] = data;
		if (d->latch_b == 0xFE) {
			ppuMapper->setVrom4KBank(4, d->reg[3]);
		}
		break;
	case 0xF000:
		if (data & 0x01)
			ppuMapper->setMirroring(NesPpuMapper::Horizontal);
		else
			ppuMapper->setMirroring(NesPpuMapper::Vertical);
		break;
	}
}

PpuMapper10::PpuMapper10(NesMapper *mapper) :
	NesPpuMapper(mapper),
	d(0) {
}

void PpuMapper10::reset() {
	d = static_cast<CpuMapper10 *>(mapper()->cpuMapper())->d;

	setVrom4KBank(0, 4);
	setVrom4KBank(4, 0);
}

void PpuMapper10::characterLatch(quint16 address) {
	if ((address&0x1FF0) == 0x0FD0 && d->latch_a != 0xFD) {
		d->latch_a = 0xFD;
		setVrom4KBank(0, d->reg[0]);
	} else if ((address&0x1FF0) == 0x0FE0 && d->latch_a != 0xFE) {
		d->latch_a = 0xFE;
		setVrom4KBank(0, d->reg[1]);
	} else if ((address&0x1FF0) == 0x1FD0 && d->latch_b != 0xFD) {
		d->latch_b = 0xFD;
		setVrom4KBank(4, d->reg[2]);
	} else if ((address&0x1FF0) == 0x1FE0 && d->latch_b != 0xFE) {
		d->latch_b = 0xFE;
		setVrom4KBank(4, d->reg[3]);
	}
}

bool CpuMapper10::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 4; i++)
		s << d->reg[i];
	s << d->latch_a;
	s << d->latch_b;
	return true;
}

bool CpuMapper10::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 4; i++)
		s >> d->reg[i];
	s >> d->latch_a;
	s >> d->latch_b;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(10, "Nintendo MMC4")
