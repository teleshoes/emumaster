#ifndef NesPpuMapper_H
#define NesPpuMapper_H

class NesMapper;
#include "nes_global.h"
#include <QObject>

class NES_EXPORT NesPpuMapper : public QObject {
	Q_OBJECT
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

	explicit NesPpuMapper(NesMapper *mapper);
	~NesPpuMapper();
	NesMapper *mapper() const;

	virtual void reset();

	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);

	virtual bool save(QDataStream &s);
	virtual bool load(QDataStream &s);

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

	BankType bank1KType(uint bank) const;
	quint8 *bank1KData(uint bank) const;
protected:
	quint8 *m_vrom;
	quint8 m_vram[4*1024];
	quint8 m_cram[32*1024];
private:
	quint8 *m_banks[16];
	BankType m_banksType[16];
	uint m_vromSize;
	Mirroring m_defaultMirroring;
};

inline void NesPpuMapper::write(quint16 address, quint8 data)
{ Q_ASSERT((address >> 10) < 16); m_banks[address >> 10][address & 0x3FF] = data; }
inline quint8 NesPpuMapper::read(quint16 address)
{ Q_ASSERT((address >> 10) < 16); return m_banks[address >> 10][address & 0x3FF]; }

inline void NesPpuMapper::setVrom8KBank(uint vromBank8K) {
	setVrom4KBank(0, vromBank8K * 2 + 0);
	setVrom4KBank(4, vromBank8K * 2 + 1);
}

inline void NesPpuMapper::setVrom4KBank(uint page, uint vromBank4K) {
	setVrom2KBank(page+0, vromBank4K * 2 + 0);
	setVrom2KBank(page+2, vromBank4K * 2 + 1);
}

inline void NesPpuMapper::setVrom2KBank(uint page, uint vromBank2K) {
	setVrom1KBank(page+0, vromBank2K * 2 + 0);
	setVrom1KBank(page+1, vromBank2K * 2 + 1);
}

inline void NesPpuMapper::setVrom1KBank(uint page, uint vromBank1K) {
	Q_ASSERT(page < 16);
	if (vromSize1KB()) {
		vromBank1K = vromBank1K % vromSize1KB();
		m_banks[page] = m_vrom + vromBank1K * 0x0400;
		m_banksType[page] = VromBank;
	} else {
		setCram1KBank(page, vromBank1K);
	}
}

inline void NesPpuMapper::setCram8KBank(uint cramBank8K) {
	setCram4KBank(0, cramBank8K * 2 + 0);
	setCram4KBank(4, cramBank8K * 2 + 1);
}

inline void NesPpuMapper::setCram4KBank(uint page, uint cramBank4K) {
	setCram2KBank(page+0, cramBank4K * 2 + 0);
	setCram2KBank(page+2, cramBank4K * 2 + 1);
}

inline void NesPpuMapper::setCram2KBank(uint page, uint cramBank2K) {
	setCram1KBank(page+0, cramBank2K * 2 + 0);
	setCram1KBank(page+1, cramBank2K * 2 + 1);
}

inline void NesPpuMapper::setCram1KBank(uint page, uint cramBank1K) {
	Q_ASSERT(page < 16);
	m_banks[page] = m_cram + (cramBank1K % (sizeof(m_cram) / 0x400)) * 0x400;
	m_banksType[page] = CramBank;
}

inline void NesPpuMapper::setVram1KBank(uint page, uint vramBank1K) {
	Q_ASSERT(page < 16);
	m_banks[page] = m_vram + (vramBank1K % (sizeof(m_vram) / 0x400)) * 0x400;
	m_banksType[page] = CramBank;
}

inline uint NesPpuMapper::vromSizeInBytes() const
{ return m_vromSize; }
inline uint NesPpuMapper::vromSize8KB() const
{ return m_vromSize / 0x2000; }
inline uint NesPpuMapper::vromSize4KB() const
{ return m_vromSize /  0x1000; }
inline uint NesPpuMapper::vromSize2KB() const
{ return m_vromSize /  0x800; }
inline uint NesPpuMapper::vromSize1KB() const
{ return m_vromSize /  0x400; }

inline NesPpuMapper::BankType NesPpuMapper::bank1KType(uint bank) const
{ Q_ASSERT(bank < 16); return m_banksType[bank]; }
inline quint8 *NesPpuMapper::bank1KData(uint bank) const
{ Q_ASSERT(bank < 16); return m_banks[bank]; }

#endif // NesPpuMapper_H
