#include "mapper241.h"

CpuMapper241::CpuMapper241(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper241::writeHigh(quint16 address, quint8 data) {
	if (address == 0x8000)
		setRomBank(data);
}

NES_MAPPER_PLUGIN_SOURCE(241, "Fon Serm Bon")
