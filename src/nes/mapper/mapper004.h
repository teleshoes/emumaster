#ifndef MAPPER004_H
#define MAPPER004_H

#include "../mapper.h"

class Mapper004 : public NesMapper {

public:
	void reset();

	u8 readLow(u16 address);
	void writeLow(u16 address, u8 data);

	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	void horizontalSync();

	void updateCpuBanks();
	void updatePpuBanks();
private:
	enum Irq {
		IrqNone = 0,
		IrqKlax,
		IrqShougimeikan,
		IrqDai2JiSuper,
		IrqDBZ2,
		IrqRockman3
	};

	u8	reg[8];
	u8	prg0, prg1;
	u8	chr01, chr23, chr4, chr5, chr6, chr7;

	u8	irq_type;
	u8	irq_enable;
	u8	irq_counter;
	u8	irq_latch;
	u8	irq_request;
	u8	irq_preset;
	u8	irq_preset_vbl;

	u8	vs_patch;
	u8	vs_index;
};

#endif // MAPPER004_H
