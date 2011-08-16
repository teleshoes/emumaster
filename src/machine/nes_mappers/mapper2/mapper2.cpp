#include "mapper2.h"
#include "nesdisk.h"

CpuMapper2::CpuMapper2(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper2::reset() {
	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	patch = 0;
	hasBattery = disk()->hasBatteryBackedRam();

	quint32 crc = disk()->crc();
	if( crc == 0x8c3d54e8		// Ikari(J)
	 || crc == 0x655efeed		// Ikari Warriors(U)
	 || crc == 0x538218b2 ) {	// Ikari Warriors(E)
		patch = 1;
	}

	if( crc == 0xb20c1030 ) {	// Shanghai(J)(original)
		patch = 2;
	}
}

void CpuMapper2::write(quint16 address, quint8 data) {
	if (!hasBattery) {
		if (address >= 0x5000 && patch == 1)
			setRom16KBank(0, data);
	} else {
		NesCpuMapper::write(address, data);
	}
}

void CpuMapper2::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	if (patch != 2)
		setRom16KBank(4, data);
	else
		setRom16KBank(4, data >> 4);
}

NES_MAPPER_PLUGIN_EXPORT(2, "UNROM")
