#ifndef NESCPUMEMORYMAPPER_H
#define NESCPUMEMORYMAPPER_H

class NesMapper;
class NesDisk;
class NesCpu;
class NesPpu;
class Nes2C0XRegisters;
class NesApu;
class NesPad;
#include "nes_global.h"
#include <QObject>

class NES_EXPORT NesCpuMemoryMapper : public QObject {
	Q_OBJECT
public:
	explicit NesCpuMemoryMapper(NesMapper *mapper);
	virtual void reset();
	virtual void write(quint16 address, quint8 data);
	virtual void writeLow(quint16 address, quint8 data);
	virtual void writeHigh(quint16 address, quint8 data);
	virtual quint8 read(quint16 address);
	NesMapper *mapper() const;

	virtual void save(QDataStream &s);
	virtual bool load(QDataStream &s);

	void setRomBank(uint bank);
	void setRom16KBank(uint i, uint bank);
	void setRom8KBank(uint i, uint bank);
	uint romSize() const;
	uint romSize16KB() const;
	uint romSize8KB() const;
	quint8 *rom8KBankData(uint bank) const;
private:
	void spriteRamDma(int page);
	void writeReg(quint16 address, quint8 data);
	quint8 readReg(quint16 address);

	quint8 *m_romData;
	uint m_romSize;

	quint8 *m_romBanks[4];
	quint8 m_ram[8*1024];
	quint8 m_wram[128*1024];

	NesDisk *m_disk;
	NesCpu *m_cpu;
	NesPpu *m_ppu;
	Nes2C0XRegisters *m_ppuRegisters;
	NesApu *m_apu;
	NesPad *m_pad;
};

inline uint NesCpuMemoryMapper::romSize() const
{ return m_romSize; }
inline uint NesCpuMemoryMapper::romSize16KB() const
{ return m_romSize / 0x4000; }
inline uint NesCpuMemoryMapper::romSize8KB() const
{ return m_romSize / 0x2000; }

inline void NesCpuMemoryMapper::setRomBank(uint bank) {
	setRom16KBank(0, bank * 2 + 0);
	setRom16KBank(1, bank * 2 + 1);
}

inline void NesCpuMemoryMapper::setRom16KBank(uint i, uint bank) {
	setRom8KBank(i * 2 + 0, bank * 2 + 0);
	setRom8KBank(i * 2 + 1, bank * 2 + 1);
}

inline void NesCpuMemoryMapper::setRom8KBank(uint i, uint bank) {
	bank = bank % romSize8KB();
	m_romBanks[i] = rom8KBankData(bank);
}

inline quint8 *NesCpuMemoryMapper::rom8KBankData(uint bank) const
{ return m_romData + bank * 0x2000; }

#endif // NESCPUMEMORYMAPPER_H
