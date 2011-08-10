#ifndef MAPPER4_H
#define MAPPER4_H

#include "nesmapper.h"
#include "nescpumemorymapper.h"

class CpuMapper4 : public NesCpuMemoryMapper {
	Q_OBJECT
public:
	enum Register {
		ControlRegister = 0x8000,
		DataRegister = 0x8001,
		MirroringRegister = 0xA000
	};

	enum ControlRegisterBit {
		CHRAddressInvert = 0x80, // When set, does an effective XOR of 1000h with the CHR addresses
		/*	0 - 8000-9FFFh and A000-BFFFh can be swapped out while C000-FFFFh is fixed.
			1 - A000-BFFFh and C000-DFFFh can be swapped out while 8000-9FFFh is fixed.
			E000-FFFF is always fixed to the last bank of ROM.
			When 0, C000-DFFF will contain the second to last bank of ROM.
			When 1, 8000-9FFF will contain the second to last bank of ROM.*/
		PRGROMSwappingControl = 0x40,
		/*	000b 0 - Select 2 1K CHR ROM pages at 0000h in PPU space
			001b 1 - Select 2 1K CHR ROM pages at 0800h in PPU space
			010b 2 - Select 1K CHR ROM page at 1000h in PPU space
			011b 3 - Select 1K CHR ROM page at 1400h in PPU space
			100b 4 - Select 1K CHR ROM page at 1800h in PPU space
			101b 5 - Select 1K CHR ROM page at 1C00h in PPU space
			110b 6 - Select 8K PRG ROM page at 8000h or C000h
			111b 7 - Select 8K PRG ROM page at A000h*/
		Mode = 0x07
	};

	void reset();
	void writeHigh(quint16 address, quint8 data);
private:
};

class PpuMapper4: public PpuMemoryMapper {
public:

};

#endif // MAPPER4_H
