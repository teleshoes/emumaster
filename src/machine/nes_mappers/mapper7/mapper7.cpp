#include "mapper7.h"
#include "nesppu.h"
#include "nesdisk.h"

CpuMapper7::CpuMapper7(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper7::reset() {
	patch = 0;
	setRom32KBank(0);
	mapper()->ppuMapper()->setMirroring(NesPpuMapper::SingleLow);

	quint32 crc = disk()->crc();
	if( crc == 0x3c9fe649 ) {	// WWF Wrestlemania Challenge(U)
		mapper()->ppuMapper()->setMirroring(NesPpuMapper::Vertical);
		patch = 1;
	}
	if( crc == 0x09874777 ) {	// Marble Madness(U)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}

	if( crc == 0x279710DC		// Battletoads (U)
	 || crc == 0xCEB65B06 ) {	// Battletoads Double Dragon (U)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
		qMemSet(m_wram, 0, sizeof(m_wram));
	}
}

void CpuMapper7::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom32KBank(data & 0x07);
	if (!patch) {
		if (data & 0x10)
			mapper()->ppuMapper()->setMirroring(NesPpuMapper::SingleHigh);
		else
			mapper()->ppuMapper()->setMirroring(NesPpuMapper::SingleLow);
	}
}

NES_MAPPER_PLUGIN_EXPORT(7, "AOROM/AMROM")
