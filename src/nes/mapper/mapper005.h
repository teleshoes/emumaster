#ifndef MAPPER005_H
#define MAPPER005_H

#include "../mapper.h"

class Mapper005 : public NesMapper {

public:
	void reset();

	u8 readLow(u16 address);
	void writeLow(u16 address, u8 data);
	void writeHigh(u16 address, u8 data);

	void setBankSram(u8 page, u8 data);
	void setBank(u16 address, u8 data);

	void updatePpuBanks();

	void horizontalSync(int scanline);
	void extensionLatchX(uint x);
	void extensionLatch(u16 address, u8 *plane1, u8 *plane2, u8 *attribute);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	enum Irq {
		IrqNone = 0,
		IrqMetal
	};
	u8	sram_size;

	u8	prg_size;		// $5100
	u8	chr_size;		// $5101
	u8	sram_we_a, sram_we_b;	// $5102-$5103
	u8	graphic_mode;		// $5104
	u8	nametable_mode;		// $5105
	u8	nametable_type[4];	// $5105 use

	u8	sram_page;		// $5113

	u8	fill_chr, fill_pal;	// $5106-$5107
	u8	split_control;		// $5200
	u8	split_scroll;		// $5201
	u8	split_page;		// $5202

	u8	split_x;
	u16	split_addr;
	u16	split_yofs;

	u8	chr_type;
	u8	chr_mode;		// $5120-$512B use
	u8	chr_page[2][8];		// $5120-$512B
	u8 *BG_MEM_BANK[8];

	u8	irq_status;		// $5204(R)
	u8	irq_enable;		// $5204(W)
	u8	irq_line;		// $5203
	u8	irq_scanline;
	u8	irq_clear;		// HSync
	u8	irq_type;

	u8	mult_a, mult_b;		// $5205-$5206

	bool cpu_bank_wren[8];
};

#endif // MAPPER005_H
