#include "mapper007.h"
#include "ppu.h"
#include "disk.h"

void Mapper007::reset() {
	NesMapper::reset();
	patch = 0;
	setRom32KBank(0);
	setMirroring(SingleLow);

	quint32 crc = disk()->crc();
	if( crc == 0x3c9fe649 ) {	// WWF Wrestlemania Challenge(U)
		setMirroring(VerticalMirroring);
		patch = 1;
	}
	if( crc == 0x09874777 ) {	// Marble Madness(U)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}

	if( crc == 0x279710DC		// Battletoads (U)
	 || crc == 0xCEB65B06 ) {	// Battletoads Double Dragon (U)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
		qMemSet(nesWram, 0, sizeof(nesWram));
	}
}

void Mapper007::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom32KBank(data & 0x07);
	if (!patch) {
		if (data & 0x10)
			setMirroring(SingleHigh);
		else
			setMirroring(SingleLow);
	}
}
