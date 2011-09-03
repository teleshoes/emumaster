#include "mapper244.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper244::CpuMapper244(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper244::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);
}

void CpuMapper244::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address >= 0x8065 && address <= 0x80A4)
		setRom32KBank((address-0x8065)&0x3);
	if (address >= 0x80A5 && address <= 0x80E4)
		ppuMapper->setVrom8KBank((address-0x80A5)&0x7);
}

NES_MAPPER_PLUGIN_EXPORT(244, "-")
