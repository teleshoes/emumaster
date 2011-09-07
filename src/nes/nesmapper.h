#ifndef MAPPER_H
#define MAPPER_H

class NesPpuRegisters;
#include "nesmachine.h"
#include "gamegeniecode.h"
#include <QObject>
#include <QtPlugin>

class NesMapper : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name CONSTANT)
public:
	enum Mirroring {
		Vertical = 0,
		Horizontal,
		SingleLow,
		SingleHigh,
		FourScreen
	};
	enum BankType {
		VromBank = 0,
		CramBank,
		VramBank
	};
	static NesMapper *create(NesMachine *machine, quint8 type);

	explicit NesMapper();
	virtual void reset();

	NesMachine *machine() const;

	QString name() const;

	NesDisk *disk() const;
	NesCpu *cpu() const;
	NesPpu *ppu() const;
	NesApu *apu() const;
	NesPpuRegisters *ppuRegisters() const;

	virtual bool save(QDataStream &s);
	virtual bool load(QDataStream &s);

	// -------- CPU Section -----------
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);

	// 0x4100-0x7FFF lower memory read/write
	virtual void writeLow(quint16 address, quint8 data);
	virtual quint8 readLow(quint16 address);

	// 0x8000-0xFFFF memory write
	virtual void writeHigh(quint16 address, quint8 data);

	// 0x4018-0x40FF extention register read/write
	virtual void writeEx(quint16 address, quint8 data);
	virtual quint8 readEx(quint16 address);

	virtual void clock(uint cycles);

	void setRom32KBank(uint romBank32K);
	void setRom16KBank(uint page, uint romBank16K);
	void setRom8KBank(uint page, uint romBank8K);
	void setRom8KBanks(uint bank4, uint bank5, uint bank6, uint bank7);

	void setWram8KBank(uint page, uint wramBank8K);

	uint romSizeInBytes() const;
	uint romSize16KB() const;
	uint romSize8KB() const;

	void setIrqSignalOut(bool on);

	void setGameGenieCodeList(const QList<GameGenieCode> &codes);
	void processGameGenieCodes();
signals:
	void request_irq_o(bool on);
protected:
	void writeDirect(quint16 address, quint8 data);
	quint8 readDirect(quint16 address) const;

	quint8 *m_rom;
	quint8 m_ram[8*1024];
	quint8 m_wram[128*1024];
	quint8 m_xram[8*1024];

	// ------- PPU Section ------
public:
	void ppuWrite(quint16 address, quint8 data);
	quint8 ppuRead(quint16 address);

	virtual void horizontalSync(int scanline);
	virtual void verticalSync();
	virtual void addressBusLatch(quint16 address);
	virtual void characterLatch(quint16 address);

	virtual void extensionLatchX(uint x);
	virtual void extensionLatch(quint16 address, quint8 *plane1, quint8 *plane2, quint8 *attribute);

	void setMirroring(Mirroring mirroring);
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

	uint vromSizeInBytes() const;
	uint vromSize8KB() const;
	uint vromSize4KB() const;
	uint vromSize2KB() const;
	uint vromSize1KB() const;

	BankType ppuBank1KType(uint bank) const;
	quint8 *ppuBank1KData(uint bank) const;
protected:
	quint8 *m_vrom;
	quint8 m_vram[4*1024];
	quint8 m_cram[32*1024];
private:
	quint8 *m_ppuBanks[16];
	BankType m_ppuBanksType[16];
	uint m_vromSize;
	Mirroring m_defaultMirroring;
private:
	void writeReg(quint16 address, quint8 data);
	quint8 readReg(quint16 address);

	quint8 *m_cpuBanks[8]; // 8K banks 0x0000-0xFFFF
	uint m_romSize;
	bool m_irqOut;

	NesDisk *m_disk;
	NesCpu *m_cpu;
	NesPpu *m_ppu;
	NesPpuRegisters *m_ppuRegisters;
	NesApu *m_apu;
	NesPad *m_pad;

	QString m_name;

	QList<GameGenieCode> m_gameGenieCodeList;
};

inline NesMachine *NesMapper::machine() const
{ return static_cast<NesMachine *>(parent()); }
inline QString NesMapper::name() const
{ return m_name; }

inline NesDisk *NesMapper::disk() const
{ return m_disk; }
inline NesCpu *NesMapper::cpu() const
{ return m_cpu; }
inline NesPpu *NesMapper::ppu() const
{ return m_ppu; }
inline NesApu *NesMapper::apu() const
{ return m_apu; }
inline NesPpuRegisters *NesMapper::ppuRegisters() const
{ return m_ppuRegisters; }

inline uint NesMapper::romSizeInBytes() const
{ return m_romSize; }
inline uint NesMapper::romSize16KB() const
{ return m_romSize / 0x4000; }
inline uint NesMapper::romSize8KB() const
{ return m_romSize / 0x2000; }

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
	m_cpuBanks[page] = m_rom + romBank8K * 0x2000;
}

inline void NesMapper::setWram8KBank(uint page, uint wramBank8K) {
	wramBank8K = wramBank8K % 128;
	m_cpuBanks[page] = m_wram + wramBank8K * 0x2000;
}

inline void NesMapper::setRom8KBanks(uint bank4, uint bank5, uint bank6, uint bank7) {
	setRom8KBank(4, bank4);
	setRom8KBank(5, bank5);
	setRom8KBank(6, bank6);
	setRom8KBank(7, bank7);
}

inline void NesMapper::writeDirect(quint16 address, quint8 data)
{ m_cpuBanks[address >> 13][address & 0x1FFF] = data; }
inline quint8 NesMapper::readDirect(quint16 address) const
{ return m_cpuBanks[address >> 13][address & 0x1FFF]; }

inline void NesMapper::ppuWrite(quint16 address, quint8 data)
{ Q_ASSERT((address >> 10) < 16); m_ppuBanks[address >> 10][address & 0x3FF] = data; }
inline quint8 NesMapper::ppuRead(quint16 address)
{ Q_ASSERT((address >> 10) < 16); return m_ppuBanks[address >> 10][address & 0x3FF]; }

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
	if (vromSize1KB()) {
		vromBank1K = vromBank1K % vromSize1KB();
		m_ppuBanks[page] = m_vrom + vromBank1K * 0x0400;
		m_ppuBanksType[page] = VromBank;
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
	m_ppuBanks[page] = m_cram + (cramBank1K % (sizeof(m_cram) / 0x400)) * 0x400;
	m_ppuBanksType[page] = CramBank;
}

inline void NesMapper::setVram1KBank(uint page, uint vramBank1K) {
	Q_ASSERT(page < 16);
	m_ppuBanks[page] = m_vram + (vramBank1K % (sizeof(m_vram) / 0x400)) * 0x400;
	m_ppuBanksType[page] = CramBank;
}

inline uint NesMapper::vromSizeInBytes() const
{ return m_vromSize; }
inline uint NesMapper::vromSize8KB() const
{ return m_vromSize / 0x2000; }
inline uint NesMapper::vromSize4KB() const
{ return m_vromSize /  0x1000; }
inline uint NesMapper::vromSize2KB() const
{ return m_vromSize /  0x800; }
inline uint NesMapper::vromSize1KB() const
{ return m_vromSize /  0x400; }

inline NesMapper::BankType NesMapper::ppuBank1KType(uint bank) const
{ Q_ASSERT(bank < 16); return m_ppuBanksType[bank]; }
inline quint8 *NesMapper::ppuBank1KData(uint bank) const
{ Q_ASSERT(bank < 16); return m_ppuBanks[bank]; }

#endif // MAPPER_H
