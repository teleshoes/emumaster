#ifndef MAPPER_H
#define MAPPER_H

#include "machine.h"
#include "gamegeniecode.h"

enum NesMirroring {
	VerticalMirroring = 0,
	HorizontalMirroring,
	SingleLow,
	SingleHigh,
	FourScreenMirroring
};

enum NesPpuBankType {
	VromBank = 0,
	CramBank,
	VramBank
};

class NesMapper {
public:
	static NesMapper *create(u8 type);

	virtual void reset();

	QString name() const;

	virtual bool save(QDataStream &s);
	virtual bool load(QDataStream &s);

	// -------- CPU Section -----------
	void write(u16 address, u8 data);
	u8 read(u16 address);

	// 0x4100-0x7FFF lower memory read/write
	virtual void writeLow(u16 address, u8 data);
	virtual u8 readLow(u16 address);

	// 0x8000-0xFFFF memory write
	virtual void writeHigh(u16 address, u8 data);

	// 0x4018-0x40FF extention register read/write
	virtual void writeEx(u16 address, u8 data);
	virtual u8 readEx(u16 address);

	virtual void clock(uint cycles);

	void setRom32KBank(uint romBank32K);
	void setRom16KBank(uint page, uint romBank16K);
	void setRom8KBank(uint page, uint romBank8K);
	void setRom8KBanks(uint bank4, uint bank5, uint bank6, uint bank7);

	void setWram8KBank(uint page, uint wramBank8K);

	void setIrqSignalOut(bool on);

	void setGameGenieCodeList(const QList<GameGenieCode> &codes);
	void processGameGenieCodes();

	void writeDirect(u16 address, u8 data);
	u8 readDirect(u16 address) const;

	// ------- PPU Section ------
public:
	void ppuWrite(u16 address, u8 data);
	u8 ppuRead(u16 address);

	virtual void horizontalSync(int scanline);
	virtual void verticalSync();
	virtual void addressBusLatch(u16 address);
	virtual void characterLatch(u16 address);

	virtual void extensionLatchX(uint x);
	virtual void extensionLatch(u16 address, u8 *plane1, u8 *plane2, u8 *attribute);

	void setMirroring(NesMirroring mirroring);
	void setMirroring(uint bank0, uint bank1, uint bank2, uint bank3);

	void setVrom8KBank(uint vromBank8K);
	void setVrom4KBank(uint page, uint vromBank4K);
	void setVrom2KBank(uint page, uint vromBank2K);
	void setVrom1KBank(uint page, uint vromBank1K);

	void setCram8KBank(uint cramBank8K);
	void setCram4KBank(uint page, uint cramBank4K);
	void setCram2KBank(uint page, uint cramBank2K);
	void setCram1KBank(uint page, uint cramBank1K);

	void setVram1KBank(uint page, uint vramBank1K);

	BankType ppuBank1KType(uint bank) const;
	u8 *ppuBank1KData(uint bank) const;
private:
	void writeReg(u16 address, u8 data);
	u8 readReg(u16 address);

	bool m_irqOut;

	QString m_name;
};

inline QString NesMapper::name() const
{ return m_name; }

inline void NesMapper::setRom32KBank(uint romBank32K) {
	setRom16KBank(4, romBank32K * 2 + 0);
	setRom16KBank(6, romBank32K * 2 + 1);
}

inline void NesMapper::setRom16KBank(uint page, uint romBank16K) {
	setRom8KBank(page+0, romBank16K * 2 + 0);
	setRom8KBank(page+1, romBank16K * 2 + 1);
}

inline void NesMapper::setRom8KBank(uint page, uint romBank8K) {
	romBank8K = romBank8K % romSize8KB();
	nesCpuBanks[page] = nesRom + romBank8K * 0x2000;
}

inline void NesMapper::setWram8KBank(uint page, uint wramBank8K) {
	wramBank8K = wramBank8K % 128;
	nesCpuBanks[page] = nesWram + wramBank8K * 0x2000;
}

inline void NesMapper::setRom8KBanks(uint bank4, uint bank5, uint bank6, uint bank7) {
	setRom8KBank(4, bank4);
	setRom8KBank(5, bank5);
	setRom8KBank(6, bank6);
	setRom8KBank(7, bank7);
}

inline void NesMapper::writeDirect(u16 address, u8 data)
{ nesCpuBanks[address >> 13][address & 0x1FFF] = data; }
inline u8 NesMapper::readDirect(u16 address) const
{ return nesCpuBanks[address >> 13][address & 0x1FFF]; }

inline void NesMapper::ppuWrite(u16 address, u8 data)
{ Q_ASSERT((address >> 10) < 16); nesPpuBanks[address >> 10][address & 0x3FF] = data; }
inline u8 NesMapper::ppuRead(u16 address)
{ Q_ASSERT((address >> 10) < 16); return nesPpuBanks[address >> 10][address & 0x3FF]; }

inline void NesMapper::setVrom8KBank(uint vromBank8K) {
	setVrom4KBank(0, vromBank8K * 2 + 0);
	setVrom4KBank(4, vromBank8K * 2 + 1);
}

inline void NesMapper::setVrom4KBank(uint page, uint vromBank4K) {
	setVrom2KBank(page+0, vromBank4K * 2 + 0);
	setVrom2KBank(page+2, vromBank4K * 2 + 1);
}

inline void NesMapper::setVrom2KBank(uint page, uint vromBank2K) {
	setVrom1KBank(page+0, vromBank2K * 2 + 0);
	setVrom1KBank(page+1, vromBank2K * 2 + 1);
}

inline void NesMapper::setVrom1KBank(uint page, uint vromBank1K) {
	Q_ASSERT(page < 16);
	if (vromSize1KB) {
		vromBank1K = vromBank1K % vromSize1KB;
		nesPpuBanks[page] = nesVrom + vromBank1K * 0x0400;
		nesPpuBanksType[page] = VromBank;
	} else {
		setCram1KBank(page, vromBank1K);
	}
}

inline void NesMapper::setCram8KBank(uint cramBank8K) {
	setCram4KBank(0, cramBank8K * 2 + 0);
	setCram4KBank(4, cramBank8K * 2 + 1);
}

inline void NesMapper::setCram4KBank(uint page, uint cramBank4K) {
	setCram2KBank(page+0, cramBank4K * 2 + 0);
	setCram2KBank(page+2, cramBank4K * 2 + 1);
}

inline void NesMapper::setCram2KBank(uint page, uint cramBank2K) {
	setCram1KBank(page+0, cramBank2K * 2 + 0);
	setCram1KBank(page+1, cramBank2K * 2 + 1);
}

inline void NesMapper::setCram1KBank(uint page, uint cramBank1K) {
	Q_ASSERT(page < 16);
	nesPpuBanks[page] = nesCram + (cramBank1K % (sizeof(nesCram) / 0x400)) * 0x400;
	nesPpuBanksType[page] = CramBank;
}

inline void NesMapper::setVram1KBank(uint page, uint vramBank1K) {
	Q_ASSERT(page < 16);
	nesPpuBanks[page] = nesVram + (vramBank1K % (sizeof(nesVram) / 0x400)) * 0x400;
	nesPpuBanksType[page] = VramBank;
}

inline NesMapper::BankType NesMapper::ppuBank1KType(uint bank) const
{ Q_ASSERT(bank < 16); return nesPpuBanksType[bank]; }
inline u8 *NesMapper::ppuBank1KData(uint bank) const
{ Q_ASSERT(bank < 16); return nesPpuBanks[bank]; }

extern uint nesRomSizeInBytes;
extern uint nesRomSize16KB;
extern uint nesRomSize8KB;

extern uint nesVromSizeInBytes;
extern uint nesVromSize8KB;
extern uint nesVromSize4KB;
extern uint nesVromSize2KB;
extern uint nesVromSize1KB;

extern u8 nesTrainer[512];
extern NesMirroring nesMirroring;
extern NesMirroring nesDefaultMirroring;

extern int nesMapperType;
extern NesMapper *nesMapper;

extern u8 *nesRom;
extern u8 nesRam[8*1024];
extern u8 nesWram[128*1024];
extern u8 nesXram[8*1024];

extern u8 *nesVrom;
extern u8 nesVram[4*1024];
extern u8 nesCram[32*1024];

extern u8 *nesPpuBanks[16];
extern NesPpuBankType nesPpuBanksType[16];
extern u8 *nesCpuBanks[8]; // 8K banks 0x0000-0xFFFF

// TODO memory cleanup

#endif // MAPPER_H
