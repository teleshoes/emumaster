#include "mapper4.h"

void CpuMapper4::reset() {
	setRom16KBank(0, 0);
	setRom16KBank(1, romSize16KB() - 1);
}

void CpuMapper4::writeHigh(quint16 address, quint8 data) {
}
