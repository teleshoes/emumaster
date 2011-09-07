#include "mapper011.h"

void Mapper011::reset() {
	NesMapper::reset();
	setRom32KBank(0);
	if (vromSize1KB())
		setVrom8KBank(0);
	setMirroring(Vertical);
}

void Mapper011::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	setRom32KBank(data);
	if (vromSize1KB())
		setVrom8KBank(data >> 4);
}
