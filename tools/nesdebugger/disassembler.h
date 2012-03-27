#ifndef NESDISASSEMBLER_H
#define NESDISASSEMBLER_H

#include <base/emu.h>

class NesInstruction
{
public:
	const char *disassembled;
	const char *comment;
	u8 op;
	u8 pad1;
	u16 pad2;
	u32 pad3;
};

class NesDisassembler
{
public:
	NesDisassembler(u8 *nesMemory);
	void clear();
	QList<u16> positions() const;
	u16 nextInstruction(u16 pc) const;
	NesInstruction instruction(u16 pc) const;
};

#endif // NESDISASSEMBLER_H
