#ifndef MAPPER016_H
#define MAPPER016_H

#include "../mapper.h"
#include "eeprom.h"

class Mapper016 : public NesMapper {

public:
	void reset();

	u8 readLow(u16 address);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void clock(uint cycles);
	void horizontalSync(int scanline);

	void writeSubA(u16 address, u8 data);
	void writeSubB(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	enum {
		IrqClock,
		IrqHSync
	};
	u8 patch;	// For Famicom Jump 2
	u8 eeprom_type;

	u8 reg[3];

	u8 irq_enable;
	int irq_counter;
	int irq_latch;
	u8 irq_type;

	X24C01 x24c01;
	X24C02 x24c02;
};

#endif // MAPPER016_H
