#include "mapper32.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper32::CpuMapper32(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper32::reset() {
	ppuMapper = mapper()->ppuMapper();

	patch = 0;
	reg = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);

	quint32 crc = disk()->crc();
	// For Major League(J)
	if (crc == 0xc0fed437) {
		patch = 1;
	}
	// For Ai Sensei no Oshiete - Watashi no Hoshi(J)
	if (crc == 0xfd3fc292) {
		setRom8KBanks(30, 31, 30, 31);
	}
}

void CpuMapper32::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xF000) {
	case 0x8000:
		if (reg & 0x02)
			setRom8KBank(6, data);
		else
			setRom8KBank(4, data);
		break;
	case 0x9000:
		reg = data;
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 0x01));
		break;
	case 0xA000:
		setRom8KBank(1, data);
		break;
	}
	switch (address & 0xF007) {
	case 0xB000:
	case 0xB001:
	case 0xB002:
	case 0xB003:
	case 0xB004:
	case 0xB005:
		ppuMapper->setVrom1KBank(address & 0x0007, data);
		break;
	case 0xB006:
		ppuMapper->setVrom1KBank(6, data);
		if (patch && (data & 0x40))
			ppuMapper->setMirroring(0, 0, 0, 1);
		break;
	case 0xB007:
		ppuMapper->setVrom1KBank(7, data);
		if (patch && (data & 0x40))
			ppuMapper->setMirroring(NesPpuMapper::SingleLow);
		break;
	}
}

bool CpuMapper32::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << reg;
	return true;
}

bool CpuMapper32::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> reg;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(32, "Irem G101")
