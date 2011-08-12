#include "mapper242.h"

CpuMapper242::CpuMapper242(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper242::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address & 0x01)
		setRomBank((address >> 3) & 0x1F);
}

NES_MAPPER_PLUGIN_EXPORT(242, "Wai Xing Zhan Shi")
