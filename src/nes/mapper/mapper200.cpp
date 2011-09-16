#include "mapper200.h"
#include "ppu.h"
#include "disk.h"
#include <QDataStream>

void Mapper200::reset() {
	NesMapper::reset();

	setRom16KBank(4, 0);
	setRom16KBank(6, 0);
	if (vromSize1KB())
		setVrom8KBank(0);
}

void Mapper200::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)

	setRom16KBank(4, address & 0x07);
	setRom16KBank(6, address & 0x07);
	setVrom8KBank(address & 0x07);

	if (address & 0x01)
		setMirroring(Vertical);
	else
		setMirroring(Horizontal);
}
