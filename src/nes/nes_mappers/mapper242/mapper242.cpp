#include "mapper242.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper242::CpuMapper242(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper242::reset() {
	setRom32KBank(0);
}

void CpuMapper242::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address & 0x01)
		setRom32KBank((address & 0xF8) >> 3);
}

NES_MAPPER_PLUGIN_EXPORT(242, "Wai Xing Zhan Shi")
