#ifndef MAPPER001_H
#define MAPPER001_H

#include "../mapper.h"

class Mapper001 : public NesMapper {

public:
	void reset();
	void writeHigh(u16 address, u8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	NesMirroring mirroringFromRegs() const;

	u8 patch;
	u8 wram_patch;
	u8 wram_bank;
	u8 wram_count;
	u16 last_addr;
	u8 reg[4];
	u8 shift, regbuf;
};

#endif // MAPPER001_H
