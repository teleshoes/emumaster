#ifndef NESPPUMEMORYMAPPER_H
#define NESPPUMEMORYMAPPER_H

class Nes2C0XPalette;
class Nes2C0XRegisters;
class NesMapper;
#include "nes_global.h"
#include "nesmachine.h"
#include <QObject>

class NES_EXPORT NesPpuMemoryMapper : public QObject {
	Q_OBJECT
public:
	enum Mirroring {
		Vertical = 0,
		Horizontal,
		SingleLow,
		SingleHigh,
		FourScreen
	};
	explicit NesPpuMemoryMapper(NesMapper *mapper);
	NesMapper *mapper() const;
	Nes2C0XRegisters *ppuRegisters() const;
	virtual void reset();
	virtual void write(quint16 address, quint8 data);
	virtual quint8 read(quint16 address);
	// TODO call this
	virtual void horizontalSync(int scanline);
	virtual void verticalSync();

	virtual void save(QDataStream &s);
	virtual bool load(QDataStream &s);

	void setMirroring(Mirroring mirroring);
	void setMirroring(int bank0, int bank1, int bank2, int bank3);

	void setRomBank(uint bank);
	void setRom4KBank(uint i, uint bank);
	void setRom2KBank(uint i, uint bank);
	void setRom1KBank(uint i, uint bank);
	void setCramBank(uint bank);
	void setCram4KBank(uint i, uint bank);
	void setCram2KBank(uint i, uint bank);
	void setCram1KBank(uint i, uint bank);
	uint romSize() const;
	uint romSize8KB() const;
	uint romSize4KB() const;
	uint romSize2KB() const;
	uint romSize1KB() const;
private:
	quint8 *m_rom;
	uint m_romSize;
	Mirroring m_defaultMirroring;

	quint8 *m_banks[16];
	quint8 m_ram[0x800];
	quint8 m_cram[0x8000];

	Nes2C0XPalette *m_ppuPalette;
	Nes2C0XRegisters *m_ppuRegisters;
};

inline void NesPpuMemoryMapper::setRomBank(uint bank) {
	setRom4KBank(0, bank * 2 + 0);
	setRom4KBank(1, bank * 2 + 1);
}

inline void NesPpuMemoryMapper::setRom4KBank(uint i, uint bank) {
	setRom2KBank(i * 2 + 0, bank * 2 + 0);
	setRom2KBank(i * 2 + 1, bank * 2 + 1);
}

inline void NesPpuMemoryMapper::setRom2KBank(uint i, uint bank) {
	setRom1KBank(i * 2 + 0, bank * 2 + 0);
	setRom1KBank(i * 2 + 1, bank * 2 + 1);
}

inline void NesPpuMemoryMapper::setRom1KBank(uint i, uint bank) {
	if (romSize1KB()) {
		bank = bank % romSize1KB();
		m_banks[i] = m_rom + bank * 0x0400;
	} else {
		setCram1KBank(i, bank);
	}
}

inline void NesPpuMemoryMapper::setCramBank(uint bank) {
	setCram4KBank(0, bank * 2 + 0);
	setCram4KBank(1, bank * 2 + 1);
}

inline void NesPpuMemoryMapper::setCram4KBank(uint i, uint bank) {
	setCram2KBank(i * 2 + 0, bank * 2 + 0);
	setCram2KBank(i * 2 + 1, bank * 2 + 1);
}

inline void NesPpuMemoryMapper::setCram2KBank(uint i, uint bank) {
	setCram1KBank(i * 2 + 0, bank * 2 + 0);
	setCram1KBank(i * 2 + 1, bank * 2 + 1);
}

inline void NesPpuMemoryMapper::setCram1KBank(uint i, uint bank)
{ m_banks[i] = m_cram + (bank % (sizeof(m_cram) / 0x400)) * 0x400; }

inline uint NesPpuMemoryMapper::romSize() const
{ return m_romSize; }
inline uint NesPpuMemoryMapper::romSize8KB() const
{ return m_romSize / 0x2000; }
inline uint NesPpuMemoryMapper::romSize4KB() const
{ return m_romSize / 0x1000; }
inline uint NesPpuMemoryMapper::romSize2KB() const
{ return m_romSize / 0x800; }
inline uint NesPpuMemoryMapper::romSize1KB() const
{ return m_romSize / 0x400; }
inline Nes2C0XRegisters *NesPpuMemoryMapper::ppuRegisters() const
{ return m_ppuRegisters; }

#endif // NESPPUMEMORYMAPPER_H
