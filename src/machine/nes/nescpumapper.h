#ifndef NesCpuMapper_H
#define NesCpuMapper_H

class NesMapper;
class NesDisk;
class NesCpu;
class NesPpu;
class NesPpuRegisters;
class NesApu;
class NesPad;
class GameGenieCode;
#include "nes_global.h"
#include <QObject>

class NES_EXPORT NesCpuMapper : public QObject {
	Q_OBJECT
public:
	explicit NesCpuMapper(NesMapper *mapper);
	~NesCpuMapper();

	NesMapper *mapper() const;
	NesDisk *disk() const;
	NesCpu *cpu() const;
	NesPpu *ppu() const;
	NesApu *apu() const;

	virtual void reset() = 0;

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

	virtual bool save(QDataStream &s);
	virtual bool load(QDataStream &s);

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
private:
	void writeReg(quint16 address, quint8 data);
	quint8 readReg(quint16 address);

	quint8 *m_banks[8]; // 8K banks 0x0000-0xFFFF

	uint m_romSize;

	NesDisk *m_disk;
	NesCpu *m_cpu;
	NesPpu *m_ppu;
	NesPpuRegisters *m_ppuRegisters;
	NesApu *m_apu;
	NesPad *m_pad;

	bool m_irqOut;
	QList<GameGenieCode> m_gameGenieCodeList;
};

inline NesDisk *NesCpuMapper::disk() const
{ return m_disk; }
inline NesCpu *NesCpuMapper::cpu() const
{ return m_cpu; }
inline NesPpu *NesCpuMapper::ppu() const
{ return m_ppu; }
inline NesApu *NesCpuMapper::apu() const
{ return m_apu; }

inline uint NesCpuMapper::romSizeInBytes() const
{ return m_romSize; }
inline uint NesCpuMapper::romSize16KB() const
{ return m_romSize / 0x4000; }
inline uint NesCpuMapper::romSize8KB() const
{ return m_romSize / 0x2000; }

inline void NesCpuMapper::setRom32KBank(uint romBank32K) {
	setRom16KBank(4, romBank32K * 2 + 0);
	setRom16KBank(6, romBank32K * 2 + 1);
}

inline void NesCpuMapper::setRom16KBank(uint page, uint romBank16K) {
	setRom8KBank(page+0, romBank16K * 2 + 0);
	setRom8KBank(page+1, romBank16K * 2 + 1);
}

inline void NesCpuMapper::setRom8KBank(uint page, uint romBank8K) {
	romBank8K = romBank8K % romSize8KB();
	m_banks[page] = m_rom + romBank8K * 0x2000;
}

inline void NesCpuMapper::setWram8KBank(uint page, uint wramBank8K) {
	wramBank8K = wramBank8K % 128;
	m_banks[page] = m_wram + wramBank8K * 0x2000;
}

inline void NesCpuMapper::setRom8KBanks(uint bank4, uint bank5, uint bank6, uint bank7) {
	setRom8KBank(4, bank4);
	setRom8KBank(5, bank5);
	setRom8KBank(6, bank6);
	setRom8KBank(7, bank7);
}

inline void NesCpuMapper::writeDirect(quint16 address, quint8 data)
{ m_banks[address >> 13][address & 0x1FFF] = data; }
inline quint8 NesCpuMapper::readDirect(quint16 address) const
{ return m_banks[address >> 13][address & 0x1FFF]; }

#endif // NesCpuMapper_H
