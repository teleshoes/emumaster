#ifndef NESDEBUG_H
#define NESDEBUG_H

#include <base/emu.h>

enum DebugEvent {
	DebugCpuOp,
	DebugNmi,
	DebugIrq,
	DebugBankSwitch,
	DebugBreakpoint
};

enum Message {
	Continue,
	InsertBreakpoint,
	RemoveBreakpoint,
	SetEventMask,
	SendCpuRegisters,
	SendCpuBanks,
	SendRomMemory,
	SendProfiler,
	ClearProfiler
};

static const u16 DefaultPort = 5496;

extern void nesDebugInit();
extern void nesDebugPostInit();
extern void nesDebugShutdown();
extern void nesDebugCpuOp(u16 pc);
extern void nesDebugNmi();
extern void nesDebugIrq();
extern void nesDebugBankSwitch(u8 page, u8 romBank);

#endif // NESDEBUG_H
