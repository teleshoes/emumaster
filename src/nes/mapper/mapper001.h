#ifndef MAPPER001_H
#define MAPPER001_H

#include "../nesmapper.h"

class Mapper001 : public NesMapper {
	Q_OBJECT
public:
	void reset();
	void writeHigh(quint16 address, quint8 data);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	Mirroring mirroringFromRegs() const;

	quint8 patch;
	quint8 wram_patch;
	quint8 wram_bank;
	quint8 wram_count;
	quint16 last_addr;
	quint8 reg[4];
	quint8 shift, regbuf;
};

#endif // MAPPER001_H
