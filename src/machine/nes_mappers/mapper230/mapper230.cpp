#include "mapper230.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper230::CpuMapper230(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper230::reset() {
	ppuMapper = mapper()->ppuMapper();

	if (rom_sw)
		rom_sw = 0;
	else
		rom_sw = 1;
	if (rom_sw)
		setRom8KBanks(0, 1, 14, 15);
	else
		setRom8KBanks(16, 17, romSize8KB()-2, romSize8KB()-1);
}

void CpuMapper230::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)

	if (rom_sw) {
		setRom8KBank(4, (data&0x07)*2+0);
		setRom8KBank(5, (data&0x07)*2+1);
	} else {
		if (data & 0x20) {
			setRom8KBank(4, (data&0x1F)*2+16);
			setRom8KBank(5, (data&0x1F)*2+17);
			setRom8KBank(6, (data&0x1F)*2+16);
			setRom8KBank(7, (data&0x1F)*2+17);
		} else {
			setRom8KBank(4, (data&0x1E)*2+16);
			setRom8KBank(5, (data&0x1E)*2+17);
			setRom8KBank(6, (data&0x1E)*2+18);
			setRom8KBank(7, (data&0x1E)*2+19);
		}
		if (data & 0x40)
			ppuMapper->setMirroring(NesPpuMapper::Vertical);
		else
			ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	}
}

bool CpuMapper230::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << rom_sw;
	return true;
}

bool CpuMapper230::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> rom_sw;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(230, "22-in-1")
