#ifndef MAPPER004_H
#define MAPPER004_H

#include "../nesmapper.h"

class Mapper004 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);

	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	void horizontalSync(int scanline);

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

	quint8	reg[8];
	quint8	prg0, prg1;
	quint8	chr01, chr23, chr4, chr5, chr6, chr7;

	quint8	irq_type;
	quint8	irq_enable;
	quint8	irq_counter;
	quint8	irq_latch;
	quint8	irq_request;
	quint8	irq_preset;
	quint8	irq_preset_vbl;

	quint8	vs_patch;
	quint8	vs_index;
};

#endif // MAPPER004_H
