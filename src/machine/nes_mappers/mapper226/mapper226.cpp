#include "mapper226.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper226::CpuMapper226(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper226::reset() {
	ppuMapper = mapper()->ppuMapper();

	setRom32KBank(0);

	reg[0] = 0;
	reg[1] = 0;
}

void CpuMapper226::writeHigh(quint16 address, quint8 data) {
	if (address & 0x001 )
		reg[1] = data;
	else
		reg[0] = data;

	if (reg[0] & 0x40)
		ppuMapper->setMirroring(NesPpuMapper::Vertical);
	else
		ppuMapper->setMirroring(NesPpuMapper::Horizontal);

	quint8 bank = ((reg[0]&0x1E)>>1)|((reg[0]&0x80)>>3)|((reg[1]&0x01)<<5);

	if (reg[0] & 0x20) {
		if (reg[0] & 0x01) {
			setRom8KBank(4, bank*4+2 );
			setRom8KBank(5, bank*4+3 );
			setRom8KBank(6, bank*4+2 );
			setRom8KBank(7, bank*4+3 );
		} else {
			setRom8KBank(4, bank*4+0 );
			setRom8KBank(5, bank*4+1 );
			setRom8KBank(6, bank*4+0 );
			setRom8KBank(7, bank*4+1 );
		}
	} else {
		setRom8KBank(4, bank*4+0 );
		setRom8KBank(5, bank*4+1 );
		setRom8KBank(6, bank*4+2 );
		setRom8KBank(7, bank*4+3 );
	}
}

bool CpuMapper226::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 2; i++)
		s << reg[i];
	return true;
}

bool CpuMapper226::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 2; i++)
		s >> reg[i];
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(226, "76-in-1")
