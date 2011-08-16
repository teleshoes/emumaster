#include "mapper244.h"

CpuMapper244::CpuMapper244(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper244::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address >= 0x8065) {
		if (address < 0x80A5)
			setRomBank((address - 0x8065) & 3);
		else if (address < 0x80E5)
			mapper()->ppuMemory()->setRomBank((address - 0x80A5) & 7);
	}
}

NES_MAPPER_PLUGIN_EXPORT(244, "Mapper244")
