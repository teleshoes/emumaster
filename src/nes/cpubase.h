#ifndef CPUBASE_H
#define CPUBASE_H

#include "sync.h"
#include <base/emu.h>

class NesCpuBaseRegisters
{
public:
	u16 pc; // program counter register
	u8 a; // accumulator register
	u8 x; // index register
	u8 y; // index register
	u8 s; // stack pointer register
	u8 p; // processor status register
};

class NesCpuBase
{
public:
	enum Interrupt {
		IrqInterrupt	= 0x01,
		NmiInterrupt	= 0x02
	};

	enum InterruptSignal {
		NmiSignal		= 0x01,
		ApuIrqSignal	= 0x02,
		MapperIrqSignal	= 0x04,
		IrqSignalMask	= ApuIrqSignal|MapperIrqSignal
	};

	static const int NmiVectorAddress		= 0xfffa;
	static const int ResetVectorAddress		= 0xfffc;
	static const int IrqVectorAddress		= 0xfffe;

	static const u16 StackBase = 0x0100;

	enum AddressingMode {
		Impli,
		Accum,
		Immed,
		Absol,
		AbsoX,
		AbsoY,
		Relat,
		Indir,
		IndiX,
		IndiY,
		ZeroP,
		ZerPX,
		ZerPY
	};

	enum StatusFlag {
		Carry		= 0x01, // carry
		Zero		= 0x02, // zero
		IrqDisable	= 0x04, // irq disable
		Decimal		= 0x08, // decimal mode (not supported on the 2A03)
		Break		= 0x10, // break command (software interrupt)
		Unused		= 0x20, // unused - always 1
		Overflow	= 0x40, // overflow
		Negative	= 0x80  // negative
	};

	enum Event {
		SaveStateEvent	=  0, // load state from base to emulation loop
		LoadStateEvent	= -1, // save state from emulation loop to base
		ExitEvent		= -2 // exit from emulation loop
	};

	virtual bool init(QString *error) { Q_UNUSED(error) return true; }
	virtual void shutdown() {}

	virtual void run(NesSync *nesSync) = 0;
	virtual void reset();

	virtual s32 ticks() const = 0;
	virtual void setSignal(InterruptSignal sig, bool on) = 0;
	virtual void dma() = 0;
	virtual void clearBank(int bankIndex) { Q_UNUSED(bankIndex) }

	virtual void sl();

	static const u8 cyclesTable[256];
	static const u8 sizeTable[256];
	static const u8 addressingModeTable[256];
	static const char *nameTable[256];
protected:
	NesCpuBaseRegisters m_stateRegs;
	int m_stateSignals;
	int m_stateInterrupts;
};

extern NesCpuBase *nesCpu;

#endif // CPUBASE_H
