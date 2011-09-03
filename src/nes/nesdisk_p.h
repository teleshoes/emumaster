#ifndef NESDISK_P_H
#define NESDISK_P_H

#include <QtGlobal>

class NesDiskHeader {
public:
	static const int FourScreenFlagA		= 0x08;
	static const int TrainerFlagA			= 0x04;
	static const int BatteryBackedRamFlagA	= 0x02;
	static const int VerticalFlagA			= 0x01;

	static const int VSSystemFlagB			= 0x01;

	char m_magic[4];
	quint8 m_num16KBRomBanks;
	quint8 m_num8KBVRomBanks;
	quint8 m_flagsA;
	quint8 m_flagsB;
	quint8 m_num8KBRamBanks;
	quint8 m_flagsC;
} Q_PACKED;

#endif // NESDISK_P_H
