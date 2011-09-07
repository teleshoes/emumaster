#ifndef MAPPER005_H
#define MAPPER005_H

#include "../nesmapper.h"

class Mapper005 : public NesMapper {
	Q_OBJECT
public:
	void reset();

	quint8 readLow(quint16 address);
	void writeLow(quint16 address, quint8 data);
	void writeHigh(quint16 address, quint8 data);

	void setBankSram(quint8 page, quint8 data);
	void setBank(quint16 address, quint8 data);

	void updatePpuBanks();

	void horizontalSync(int scanline);
	void extensionLatchX(uint x);
	void extensionLatch(quint16 address, quint8 *plane1, quint8 *plane2, quint8 *attribute);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	enum Irq {
		IrqNone = 0,
		IrqMetal
	};
	quint8	sram_size;

	quint8	prg_size;		// $5100
	quint8	chr_size;		// $5101
	quint8	sram_we_a, sram_we_b;	// $5102-$5103
	quint8	graphic_mode;		// $5104
	quint8	nametable_mode;		// $5105
	quint8	nametable_type[4];	// $5105 use

	quint8	sram_page;		// $5113

	quint8	fill_chr, fill_pal;	// $5106-$5107
	quint8	split_control;		// $5200
	quint8	split_scroll;		// $5201
	quint8	split_page;		// $5202

	quint8	split_x;
	quint16	split_addr;
	quint16	split_yofs;

	quint8	chr_type;
	quint8	chr_mode;		// $5120-$512B use
	quint8	chr_page[2][8];		// $5120-$512B
	quint8 *BG_MEM_BANK[8];

	quint8	irq_status;		// $5204(R)
	quint8	irq_enable;		// $5204(W)
	quint8	irq_line;		// $5203
	quint8	irq_scanline;
	quint8	irq_clear;		// HSync
	quint8	irq_type;

	quint8	mult_a, mult_b;		// $5205-$5206

	bool cpu_bank_wren[8];
};

#endif // MAPPER005_H
