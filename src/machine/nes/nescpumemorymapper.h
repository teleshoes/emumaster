#ifndef NESCPUMEMORYMAPPER_H
#define NESCPUMEMORYMAPPER_H

class NesMapper;
class Nes2C0XRegisters;
class NesPad;
class NesCpu;
class NesApu;
#include "nes_global.h"
#include <QObject>

class NES_EXPORT NesCpuMemoryMapper : public QObject {
	Q_OBJECT
public:
	explicit NesCpuMemoryMapper(NesMapper *mapper);
	NesMapper *mapper() const;

	virtual void write(quint16 address, quint8 data);
	virtual void writeHigh(quint16 address, quint8 data);
	virtual quint8 read(quint16 address);

	void setRomBank(uint bank);
	void setRom16KBank(uint i, uint bank);
	void setRom8KBank(uint i, uint bank);
	uint romSize() const;
	uint romSize16KB() const;
	uint romSize8KB() const;
private:
	void spriteRamDma(int page);

	quint8 *m_romData;
	uint m_romSize;

	quint8 *m_romBanks[4];
	quint8 m_ram[0x800];
	quint8 m_last4016Data;

	NesCpu *m_cpu;
	Nes2C0XRegisters *m_ppuRegisters;
	NesPad *m_pad0;
	NesPad *m_pad1;
	NesApu *m_apu;
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
	m_romBanks[i] = m_romData + bank * 0x2000;
}

#endif // NESCPUMEMORYMAPPER_H
