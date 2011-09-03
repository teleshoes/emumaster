#include "mapper202.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper202::CpuMapper202(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper202::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom16KBank(4, 6);
	setRom16KBank(6, 7);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper202::writeEx(quint16 address, quint8 data) {
	if (address >= 0x4020)
		writeSub(address, data);
}

void CpuMapper202::writeLow(quint16 address, quint8 data) {
	writeSub(address, data);
}

void CpuMapper202::writeHigh(quint16 address, quint8 data) {
	writeSub(address, data);
}

void CpuMapper202::writeSub(quint16 address, quint8 data) {
	Q_UNUSED(data)

	int bank = (address>>1) & 0x07;
	setRom16KBank(4, bank);
	if ((address & 0x0C) == 0x0C)
		setRom16KBank(6, bank+1);
	else
		setRom16KBank(6, bank);
	ppuMapper->setVrom8KBank(bank);

	if (address & 0x01)
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	else
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
}

NES_MAPPER_PLUGIN_EXPORT(202, "150-in-1")
