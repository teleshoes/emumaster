#include "mapper2.h"
#include "nesdisk.h"

CpuMapper2::CpuMapper2(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper2::reset() {
	NesCpuMemoryMapper::reset();
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB() - 1);
	m_patch = 0;
	m_hasBattery = mapper()->machine()->disk()->hasBatteryBackedRam();
//	TODO DWORD	crc = nes->rom->GetPROM_CRC();
////	if( crc == 0x322c9b09 ) {	// Metal Gear (Alt)(J)
//////		nes->SetFrameIRQmode( FALSE );
////	}
////	if( crc == 0xe7a3867b ) {	// Dragon Quest 2(Alt)(J)
////		nes->SetFrameIRQmode( FALSE );
////	}
//////	if( crc == 0x9622fbd9 ) {	// Ballblazer(J)
//////		patch = 0;
//////	}
//	if( crc == 0x8c3d54e8		// Ikari(J)
//	 || crc == 0x655efeed		// Ikari Warriors(U)
//	 || crc == 0x538218b2 ) {	// Ikari Warriors(E)
//		patch = 1;
//	}

//	if( crc == 0xb20c1030 ) {	// Shanghai(J)(original)
//		patch = 2;
//	}
}

void CpuMapper2::write(quint16 address, quint8 data) {
	if (!m_hasBattery && address >= 0x5000 && m_patch == 1)
		setRom16KBank(0, data);
	else
		NesCpuMemoryMapper::write(address, data);
}

void CpuMapper2::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	if (m_patch != 2)
		setRom16KBank(0, data);
	else
		setRom16KBank(0, data >> 4);
}

NES_MAPPER_PLUGIN_EXPORT(2, "UNROM")
