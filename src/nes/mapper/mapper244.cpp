#include "mapper244.h"
#include "nesppu.h"
#include "nesdisk.h"

void Mapper244::reset() {
	NesMapper::reset();

	setRom32KBank(0);
}

void Mapper244::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(data)
	if (address >= 0x8065 && address <= 0x80A4)
		setRom32KBank((address-0x8065)&0x3);
	if (address >= 0x80A5 && address <= 0x80E4)
		setVrom8KBank((address-0x80A5)&0x7);
}
