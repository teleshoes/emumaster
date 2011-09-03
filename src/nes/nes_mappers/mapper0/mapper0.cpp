#include "mapper0.h"
#include "nesppu.h"
#include "nesdisk.h"

CpuMapper0::CpuMapper0(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper0::reset() {
	setRom32KBank(0);

	quint32 crc = disk()->crc();
	if (crc == 0x4e7db5af) {	// Circus Charlie(J)
		ppu()->setRenderMethod(NesPpu::PostRender);
	}
	if (crc == 0x57970078) {	// F-1 Race(J)
		ppu()->setRenderMethod(NesPpu::PostRender);
	}
	if (crc == 0xaf2bbcbc		// Mach Rider(JU)
	 || crc == 0x3acd4bf1) {	// Mach Rider(Alt)(JU)
		ppu()->setRenderMethod(NesPpu::PostRender);
	}
}

NES_MAPPER_PLUGIN_EXPORT(0, "-")
