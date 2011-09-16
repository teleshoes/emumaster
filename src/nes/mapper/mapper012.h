#ifndef MAPPER012_H
#define MAPPER012_H

#include "../mapper.h"

class Mapper012 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	void writeLow(quint16 address, quint8 data);
	quint8 readLow(quint16 address);
	void writeHigh(quint16 address, quint8 data);

	void horizontalSync(int scanline);

	void updatePpuBanks();
	void updateCpuBanks();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	quint32 vb0, vb1;
	quint8	reg[8];
	quint8	prg0, prg1;
	quint8	chr01, chr23, chr4, chr5, chr6, chr7;

	quint8	irq_enable;
	quint8	irq_counter;
	quint8	irq_latch;
	quint8	irq_request;
	quint8	irq_preset;
	quint8	irq_preset_vbl;
};

#endif // MAPPER012_H
