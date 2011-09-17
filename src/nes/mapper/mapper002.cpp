#include "mapper002.h"
#include "disk.h"

void Mapper002::reset() {
	NesMapper::reset();

	setRom8KBanks(0, 1, nesRomSize8KB-2, nesRomSize8KB-1);
	patch = 0;
	hasBattery = nesDisk.hasBatteryBackedRam();

	u32 crc = nesDiskCrc;
	if( crc == 0x8c3d54e8		// Ikari(J)
	 || crc == 0x655efeed		// Ikari Warriors(U)
	 || crc == 0x538218b2 ) {	// Ikari Warriors(E)
		patch = 1;
	}
	if( crc == 0xb20c1030 ) {	// Shanghai(J)(original)
		patch = 2;
	}
}

void Mapper002::writeLow(u16 address, u8 data) {
	if (!hasBattery) {
		if (address >= 0x5000 && patch == 1)
			setRom16KBank(0, data);
	} else {
		NesMapper::writeLow(address, data);
	}
}

void Mapper002::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	if (patch != 2)
		setRom16KBank(4, data);
	else
		setRom16KBank(4, data >> 4);
}
