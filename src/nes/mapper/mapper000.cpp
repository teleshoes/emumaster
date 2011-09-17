#include "mapper000.h"
#include "ppu.h"
#include "disk.h"

void Mapper000::reset() {
	NesMapper::reset();

	setRom32KBank(0);

	u32 crc = nesDiskCrc;
	if (crc == 0x4e7db5af) {	// Circus Charlie(J)
		nesPpu.setRenderMethod(NesPpu::PostRender);
	}
	if (crc == 0x57970078) {	// F-1 Race(J)
		nesPpu.setRenderMethod(NesPpu::PostRender);
	}
	if (crc == 0xaf2bbcbc		// Mach Rider(JU)
	 || crc == 0x3acd4bf1) {	// Mach Rider(Alt)(JU)
		nesPpu.setRenderMethod(NesPpu::PostRender);
	}
}
