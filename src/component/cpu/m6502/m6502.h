#ifndef M6502_H
#define M6502_H

#include "m6502_global.h"
#include <QObject>

class M6502_EXPORT M6502 : public QObject {
	Q_OBJECT
public:
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
	explicit M6502(QObject *parent = 0);
	void setPC(quint16 address);
	void stoleCycles(uint n);
	quint64 cycle() const;
	void executeOne();
	void clockTo(quint64 endCycle);

	static const char *instructionName(quint8 instruction);
	static int instructionSize(quint8 instruction);
	static AddressingMode addressingMode(quint8 instruction);
public slots:
	void irq0_i(bool on);
	void nmi_i(bool on);
	virtual void reset_i(bool on);
protected:
	virtual void write(quint16 address, quint8 data) = 0;
	virtual quint8 read(quint16 address) = 0;
private:
	enum SignalIn { Irq0 = 1,	Nmi = 2, Reset = 8 };
	Q_DECLARE_FLAGS(SignalsIn, SignalIn)

	void setSignal(SignalIn sig, bool on);

	void execute(quint8 instr);

	void WRITE(quint16 addr, quint8 data);
	quint8 READ(quint16 addr);
	void PUSH(quint8 data);
	quint8 M6502::POP();
	void ADDCYC(uint n);

	static const int StackBase = 0x100;

	quint8 ZNTable[256];

	SignalsIn m_signals;
	quint8 A; // accumulator register
	quint8 Y, X; // index registers
	quint16 PC; // program counter register
	quint8 S; // stack pointer register
	quint8 P; // processor status register
	bool m_nmiState;

	quint64 m_cycle;

	static const quint8 cyclesTable[256];
	static const quint8 sizeTable[256];
	static const quint8 addressingModeTable[256];
	static const char *nameTable[256];
};


inline void M6502::WRITE(quint16 addr, quint8 data)
{ write(addr,data); }
inline quint8 M6502::READ(quint16 addr)
{ return read(addr); }

inline void M6502::PUSH(quint8 data)
{ WRITE(StackBase+S, data); S--; }
inline quint8 M6502::POP()
{ return READ(StackBase + (++S)); }

inline void M6502::stoleCycles(uint n)
{ m_cycle += n; }
inline quint64 M6502::cycle() const
{ return m_cycle; }

inline const char *M6502::instructionName(quint8 instruction)
{ return nameTable[instruction]; }
inline int M6502::instructionSize(quint8 instruction)
{ return sizeTable[instruction]; }
inline M6502::AddressingMode M6502::addressingMode(quint8 instruction)
{ return static_cast<AddressingMode>(addressingModeTable[instruction]); }

#endif // M6502_H
