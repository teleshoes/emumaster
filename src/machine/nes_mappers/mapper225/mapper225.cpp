#include "mapper225.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper225::CpuMapper225(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper225::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);
	if (ppuMapper->vromSize1KB())
		ppuMapper->setVrom8KBank(0);
}

void CpuMapper225::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	quint8 prg_bank = (address & 0x0F80) >> 7;
	quint8 chr_bank = address & 0x003F;

	ppuMapper->setVrom1KBank(0, chr_bank*8+0);
	ppuMapper->setVrom1KBank(1, chr_bank*8+1);
	ppuMapper->setVrom1KBank(2, chr_bank*8+2);
	ppuMapper->setVrom1KBank(3, chr_bank*8+3);
	ppuMapper->setVrom1KBank(4, chr_bank*8+4);
	ppuMapper->setVrom1KBank(5, chr_bank*8+5);
	ppuMapper->setVrom1KBank(6, chr_bank*8+6);
	ppuMapper->setVrom1KBank(7, chr_bank*8+7);

	if (address & 0x2000)
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);
	else
		ppuMapper->setMirroring(NesPpuMapper::Vertical);

	if (address & 0x1000) {
		if (address & 0x0040) {
			setRom8KBank(4, prg_bank*4+2);
			setRom8KBank(5, prg_bank*4+3);
			setRom8KBank(6, prg_bank*4+2);
			setRom8KBank(7, prg_bank*4+3);
		} else {
			setRom8KBank(4, prg_bank*4+0);
			setRom8KBank(5, prg_bank*4+1);
			setRom8KBank(6, prg_bank*4+0);
			setRom8KBank(7, prg_bank*4+1);
		}
	} else {
		setRom8KBank(4, prg_bank*4+0);
		setRom8KBank(5, prg_bank*4+1);
		setRom8KBank(6, prg_bank*4+2);
		setRom8KBank(7, prg_bank*4+3);
	}
}

NES_MAPPER_PLUGIN_EXPORT(225, "72-in-1")
