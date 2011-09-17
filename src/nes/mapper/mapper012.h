#ifndef MAPPER012_H
#define MAPPER012_H

#include "../mapper.h"

class Mapper012 : public NesMapper {

public:
	void reset();

	void writeLow(u16 address, u8 data);
	u8 readLow(u16 address);
	void writeHigh(u16 address, u8 data);

	void horizontalSync(int scanline);

	void updatePpuBanks();
	void updateCpuBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	u32 vb0, vb1;
	u8	reg[8];
	u8	prg0, prg1;
	u8	chr01, chr23, chr4, chr5, chr6, chr7;

	u8	irq_enable;
	u8	irq_counter;
	u8	irq_latch;
	u8	irq_request;
	u8	irq_preset;
	u8	irq_preset_vbl;
};

#endif // MAPPER012_H
