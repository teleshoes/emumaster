#ifndef NESCPU_H
#define NESCPU_H

#include "mapper.h"

class NesCpu : public QObject {
	Q_OBJECT
public:
	static const int NmiVectorAddress		= 0xFFFA;
	static const int ResetVectorAddress		= 0xFFFC;
	static const int IrqVectorAddress		= 0xFFFE;

	enum StatusFlag {
		N = 0x80, // Negative
		V = 0x40, // Overflow
		U = 0x20, // Unused
		B = 0x10, // BrkCommand
		D = 0x08, // DecimalMode
		I = 0x04, // IrqDisable
		Z = 0x02, // Zero
		C = 0x01  // Carry
	};
	Q_DECLARE_FLAGS(StatusFlags, StatusFlag)

	enum AddressingMode {
		Impli = 0,
		Accum = 1,
		Immed = 2,
		Absol = 3,
		AbsoX = 4,
		AbsoY = 5,
		Relat = 6,
		Indir = 7,
		IndiX = 8,
		IndiY = 9,
		ZeroP = 10,
		ZerPX = 11,
		ZerPY = 12
	};
	void init();

	u32 executeOne();

	u32 clock(u32 cycles);
	void dma(u32 cycles);

	void WRITE(u16 address, u8 data);
	quint8 READ(u16 address);
	void ADDCYC(u32 n);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	void apu_irq_i(bool on);
	void irq0_i(bool on);
	void nmi_i(bool on);
	void reset_i(bool on);
	void mapper_irq_i(bool on);
private:
	enum SignalIn { Irq0 = 1, Nmi = 2, Reset = 8 };
	Q_DECLARE_FLAGS(SignalsIn, SignalIn)

	void setSignal(SignalIn sig, bool on);
	void execute(u8 instr);

	u32 m_signals;
	bool m_nmiState;
	u32 m_currentCycles;

	bool m_reset;
	u32 m_dmaCycles;

	bool m_apuIrq;
	bool m_mapperIrq;

	static const u8 cyclesTable[256];
	static const u8 sizeTable[256];
	static const u8 addressingModeTable[256];
	static const char *nameTable[256];
};

void NesCpu::WRITE(u16 address, u8 data)
{ nesMapper->write(address, data); }
u8 NesCpu::READ(u16 address)
{ return nesMapper->read(address); }
inline void NesCpu::ADDCYC(u32 n)
{ m_currentCycles += n; }

extern NesCpu nesCpu;

#endif // NESCPU_H
