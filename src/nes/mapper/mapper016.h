#ifndef MAPPER016_H
#define MAPPER016_H

#include "../nesmapper.h"
#include "nesmappereeprom.h"

class Mapper016 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void clock(uint cycles);
	void horizontalSync(int scanline);

	void writeSubA(quint16 address, quint8 data);
	void writeSubB(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	enum {
		IrqClock,
		IrqHSync
	};
	quint8 patch;	// For Famicom Jump 2
	quint8 eeprom_type;

	quint8 reg[3];

	quint8 irq_enable;
	int irq_counter;
	int irq_latch;
	quint8 irq_type;

	X24C01 x24c01;
	X24C02 x24c02;
};

#endif // MAPPER016_H
