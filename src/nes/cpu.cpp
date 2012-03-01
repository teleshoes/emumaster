/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "cpu.h"
#include "nes.h"
#include "mapper.h"
#include "apu.h"

#include <emu.h>
#include <QDataStream>

static const int NmiVectorAddress		= 0xFFFA;
static const int ResetVectorAddress		= 0xFFFC;
static const int IrqVectorAddress		= 0xFFFE;

enum StatusFlag {
	C = 0x01, // carry
	Z = 0x02, // zero
	I = 0x04, // irq disable
	D = 0x08, // decimal mode (not supported on the 2A03)
	B = 0x10, // break command (software interrupt)
	U = 0x20, // unused - always 1
	V = 0x40, // overflow
	N = 0x80  // negative
};

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

NesCpu nesCpu;

static const int StackBase = 0x100;

static u8 A; // accumulator register
static u8 Y, X; // index registers
static u16 PC; // program counter register
static u8 S; // stack pointer register
static u8 P; // processor status register

static u16	EA; // effective address
static u16	ET; // effective address temp
//     u16	WT; // word temp
static u8	DT; // data

enum SignalIn { Irq0 = 1, Nmi = 2 };

static u32 cpuSignals;
static bool nmiState;
static u32 executedCycles;
static u32 instructionCycles;

static u32 dmaCycles;

static bool apuIrq;
static bool mapperIrq;

static void execute(u8 opcode);
static inline void HW_INTERRUPT(u16 vector);

static inline void WRITE8(u16 address, u8 data)
{
	nesMapper->write(address, data);
}

static inline u8 READ8(u16 address)
{
	return nesMapper->read(address);
}

static inline u16 READ16(u16 address)
{
	u16 ret = nesMapper->read(address);
	ret |= nesMapper->read(address+1) << 8;
	return ret;
}

static inline void PUSH8(u8 data)
{
	nesRam[StackBase+S] = data;
	S--;
}

static inline u8 POP8()
{
	S++;
	return nesRam[StackBase+S];
}

static inline void PUSH16(u16 data)
{
	PUSH8(data>>8);
	PUSH8(data);
}

static inline u16 POP16()
{
	u16 ret = POP8();
	ret |= POP8() << 8;
	return ret;
}

static inline u8 FETCH_PC8()
{
	u8 ret = READ8(PC);
	PC++;
	return ret;
}

static inline u16 FETCH_PC16()
{
	u16 ret = READ16(PC);
	PC += 2;
	return ret;
}

static inline u8 FETCH_ZP8(u8 address)
{
	return nesRam[address];
}

static inline u16 FETCH_ZP16(u8 address)
{
	u16 lo = nesRam[address];
	u16 hi = nesRam[(address+1) & 0xFF];
	return lo | (hi<<8);
}

static inline void ADDCYC(u32 n)
{
	instructionCycles += n;
}

void NesCpu::init()
{
	nmiState = false;
}

static inline void executeOne()
{
	u8 opcode = READ8(PC);
	instructionCycles = NesCpu::cyclesTable[opcode];
	PC++;

	u16 vector = 0;
	if (cpuSignals) {
		if ((cpuSignals & Nmi) || !(P & I)) {
			if (cpuSignals & Nmi) {
				vector = NmiVectorAddress;
				cpuSignals &= ~Nmi;
			} else {
				vector = IrqVectorAddress;
			}
		}
	}

	execute(opcode);

	if (vector)
		HW_INTERRUPT(vector);
}

u32 NesCpu::clock(u32 cycles)
{
	executedCycles = 0;
	while (executedCycles < cycles) {
		if (dmaCycles) {
			u32 remaining = cycles - executedCycles;
			if (remaining <= dmaCycles) {
				dmaCycles -= remaining;
				executedCycles += remaining;
				nesMapper->clock(remaining);
				break;
			} else {
				executedCycles += dmaCycles;
				nesMapper->clock(dmaCycles);
				dmaCycles = 0;
			}
		}
		executeOne();
		executedCycles += instructionCycles;
		// TODO mapper clock enable
		nesMapper->clock(instructionCycles);
	}
	nesApuClock(executedCycles);
	return executedCycles;
}

void NesCpu::dma(u32 cycles)
{
	dmaCycles += cycles;
}

u32 NesCpu::ticks() const
{
	return executedCycles;
}

static inline void setSignal(SignalIn sig, bool on)
{
	if (on)
		cpuSignals |=  sig;
	else
		cpuSignals &= ~sig;
}

void NesCpu::apu_irq_i(bool on)
{
	bool oldIrqState = (apuIrq || mapperIrq);
	apuIrq = on;
	bool newIrqState = (apuIrq || mapperIrq);
	if (newIrqState != oldIrqState)
		irq0_i(newIrqState);
}

void NesCpu::mapper_irq_i(bool on) {

	bool oldIrqState = (apuIrq || mapperIrq);
	mapperIrq = on;
	bool newIrqState = (apuIrq || mapperIrq);
	if (newIrqState != oldIrqState)
		irq0_i(newIrqState);
}

void NesCpu::irq0_i(bool on)
{
	setSignal(Irq0, on);
}

void NesCpu::nmi_i(bool on)
{
	if (on && !nmiState)
		setSignal(Nmi, true);
	nmiState = on;
}

void NesCpu::reset()
{
	A = 0;
	Y = 0;
	X = 0;
	S = 0xFF;
	P = I;

	apuIrq = false;
	mapperIrq = false;
	irq0_i(false);
	dmaCycles = 0;
	cpuSignals = 0;
	executedCycles = 0;

	PC = READ16(ResetVectorAddress);
}

void NesCpu::sl()
{
	emsl.begin("cpu");
	emsl.var("dmaCycles", dmaCycles);
	emsl.var("apuIrq", apuIrq);
	emsl.var("mapperIrq", mapperIrq);
	emsl.var("cpuSignals", cpuSignals);
	emsl.var("nmiState", nmiState);
	emsl.var("A", A);
	emsl.var("Y", Y);
	emsl.var("X", X);
	emsl.var("PC", PC);
	emsl.var("S", S);
	emsl.var("P", P);
	emsl.end();
}

static inline u8 calcZN(u8 val)
{
	u8 zn = val & N;
	if (!val)
		zn |= Z;
	return zn;
}

static inline void X_ZN(u8 val)
{
	P &= ~(Z|N);
	P |= calcZN(val);
}

static inline void X_ZNT(u8 val)
{
	P |= calcZN(val);
}

static inline bool CHECK_EA()
{
	// (ET^EA) & 0x100
	bool boundary = ((ET^EA) >> 8) & 1;
	if (boundary) {
		READ8(EA-0x100);
		ADDCYC(1);
	}
	return boundary;
}

static inline void NOP() {}

//----------------------------- load instructions ------------------------------

static inline void LDA() { A = DT; X_ZN(DT); }
static inline void LDX() { X = DT; X_ZN(DT); }
static inline void LDY() { Y = DT; X_ZN(DT); }

//---------------------------- store instructions ------------------------------

static inline void STA() { DT = A; }
static inline void STX() { DT = X; }
static inline void STY() { DT = Y; }

//----------------------------- move instructions ------------------------------

static inline void TAX() { X = A; X_ZN(A); }
static inline void TXA() { A = X; X_ZN(A); }
static inline void TAY() { Y = A; X_ZN(A); }
static inline void TYA() { A = Y; X_ZN(A); }

static inline void TSX() { X = S; X_ZN(X); }
static inline void TXS() { S = X; }

//----------------------------- flow instructions ------------------------------

static inline void JMP_ABS()
{
	PC = READ16(PC);
}

static inline void JMP_IND()
{
	u16 WT = READ16(PC);
	EA = READ8(WT);
	WT = ((WT+1)&0x00FF) | (WT&0xFF00);
	PC = EA | (READ8(WT)<<8);
}

static inline void JSR()
{
	EA = READ16(PC);
	PUSH16(PC+1);
	PC = EA;
}

static inline void RTI()
{
	P = POP8();
	PC = POP16();
}

static inline void RTS()
{
	PC = POP16() + 1;
}

static inline void BRK()
{
	PUSH16(PC+1);
	PUSH8(P|U|B);
	P |= I;
	PC = READ16(IrqVectorAddress);
}

static inline void HW_INTERRUPT(u16 vector)
{
	PUSH16(PC);
	PUSH8((P&~B) | U);
	P |= I;
	PC = READ16(vector);
	ADDCYC(7);
}

static inline void BRANCH()
{
	ET = PC;
	EA = PC + (s8)DT;
	PC = EA;
	ADDCYC(1);
	CHECK_EA();
}

static inline void BCC() { if (!(P&C)) BRANCH(); }
static inline void BCS() { if (  P&C ) BRANCH(); }
static inline void BNE() { if (!(P&Z)) BRANCH(); }
static inline void BEQ() { if (  P&Z ) BRANCH(); }
static inline void BPL() { if (!(P&N)) BRANCH(); }
static inline void BMI() { if (  P&N ) BRANCH(); }
static inline void BVC() { if (!(P&V)) BRANCH(); }
static inline void BVS() { if (  P&V ) BRANCH(); }

//----------------------------- math instructions ------------------------------

static inline void ADC()
{
	// the N2A03 has no BCD mode
	u16 WT = A+DT+(P&C);
	P &= ~(V|C|N|Z);
	P |= (~(A^DT) & (A^WT) & 0x80) >> 1;
	P |= (WT>>8)&C;
	A = WT;
	X_ZNT(A);
}

static inline void SBC()
{
	DT = ~DT;
	ADC();
}

//--------------------------- logical instructions -----------------------------

static inline void AND() { A &= DT; X_ZN(A); }
static inline void ORA() { A |= DT; X_ZN(A); }
static inline void EOR() { A ^= DT; X_ZN(A); }

static inline void BIT()
{
	P &= ~(V|N|Z);
	if (!(DT & A))
		P |= Z;
	P |= DT & (V|N);
}

static inline void CMP_GENERIC(u8 reg)
{
	u16 WT = (u16)reg - (u16)DT;
	P &= ~(C|N|Z);
	P |= ((WT>>8)&C) ^ C;
	X_ZNT(WT);
}

static inline void CMP() { CMP_GENERIC(A); }
static inline void CPX() { CMP_GENERIC(X); }
static inline void CPY() { CMP_GENERIC(Y); }

//---------------------------- shift instructions ------------------------------

static inline void ASL()
{
	P &= ~(C|N|Z);
	P |= DT>>7;
	DT <<= 1;
	X_ZNT(DT);
}

static inline void LSR()
{
	P &= ~(C|N|Z);
	P |= DT&1;
	DT >>= 1;
	X_ZNT(DT);
}

static inline void ROL()
{
	u8 c = DT>>7;
	DT <<= 1;
	DT |= P&C;
	P &= ~(Z|N|C);
	P |= c;
	X_ZNT(DT);
}

static inline void ROR()
{
	u8 c = DT&1;
	DT >>= 1;
	DT |= (P&C) << 7;
	P &= ~(Z|N|C);
	P |= c;
	X_ZNT(DT);
}

//--------------------------- inc/dec instructions -----------------------------

static inline void INC() { DT++; X_ZN(DT); }
static inline void INX() { X++;  X_ZN(X);  }
static inline void INY() { Y++;  X_ZN(Y);  }

static inline void DEC() { DT--; X_ZN(DT); }
static inline void DEX() { X--;  X_ZN(X);  }
static inline void DEY() { Y--;  X_ZN(Y);  }

//---------------------------- flags instructions ------------------------------

static inline void CLC() { P &= ~C; }
static inline void CLD() { P &= ~D; }
static inline void CLI() { P &= ~I; }
static inline void CLV() { P &= ~V; }

static inline void SEC() { P |=  C; }
static inline void SED() { P |=  D; }
static inline void SEI() { P |=  I; }

//---------------------------- stack instructions ------------------------------

static inline void PHA() { PUSH8(A); }
static inline void PHP() { PUSH8(P|U|B); }
static inline void PLA() { A = POP8(); X_ZN(A); }
static inline void PLP() { P = POP8(); }

//------------------------ undocummented instructions --------------------------

static inline void AAC()
{
	AND();
	P &= ~C;
	P |= A>>7;
}

static inline void XAA()
{
	A = (A|0xEE) & X & DT;
	X_ZN(A);
}

static inline void ARR()
{
	DT &= A;
	A = (DT>>1) | ((P&C)<<7);
	P &= ~(V|C|N|Z);
	P |= (A^(A>>1)) & V;
	P |= (A>>6) & C;
	X_ZNT(A);
}

static inline void ASR() // AND,LSRA
{
	DT &= A;
	P &= ~(C|N|Z);
	P |= DT&C;
	DT >>= 1;
	A = DT;
	X_ZNT(A);
}

static inline void DCP() { DEC(); CMP(); }
static inline void ISB() { INC(); SBC(); }
static inline void LAX() { LDA(); LDX(); }
static inline void RLA() { ROL(); AND(); }
static inline void RRA() { ROR(); ADC(); }
static inline void SLO() { ASL(); ORA(); }
static inline void SRE() { LSR(); EOR(); }

static inline void LAR()
{
	S &= DT;
	A = X = S;
	X_ZN(X);
}

static inline void XAS()
{
	S = A & X;
	DT = S & ((EA>>8)+1);
}

static inline void AXS()
{
	u16 WT = (u16)(A&X) - (u16)DT;
	X = WT;
	P &= ~(C|N|Z);
	P |= ((WT>>8)&C) ^ C;
	X_ZNT(X);
}

static inline void AAX() { DT = A&X; }
static inline void ATX() { A |= 0xEE; AND(); X = A; }

static inline void SYA() { DT = Y & ((EA>>8)+1); }
static inline void SXA() { DT = X & ((EA>>8)+1); }

static inline void DOP() { PC++; }
static inline void TOP() { PC+=2; }
static inline void KIL() { ADDCYC(0xFF); PC--; }

static inline void SHA() { DT = A & X & ((EA>>8)+1); }

//--------------------------- immediate addressing -----------------------------

static inline void IMM_R()
{
	DT = FETCH_PC8();
}

//--------------------------- zero page addressing -----------------------------

static inline void ZPG_W()
{
	EA = FETCH_PC8();
}

static inline void ZPG_R()
{
	ZPG_W();
	DT = FETCH_ZP8(EA);
}

static inline void ZPG_RW()
{
	ZPG_R();
}

//----------------------- zero page indexed addressing -------------------------

static inline void ZPI_W(u8 reg)
{
	DT = FETCH_PC8();
	EA = (u8)(DT+reg);
}

static inline void ZPI_R(u8 reg)
{
	ZPI_W(reg);
	DT = FETCH_ZP8(EA);
}

static inline void ZPX_W() { ZPI_W(X); }
static inline void ZPY_W() { ZPI_W(Y); }

static inline void ZPX_R() { ZPI_R(X); }
static inline void ZPY_R() { ZPI_R(Y); }

static inline void ZPX_RW() { ZPI_R(X); }
static inline void ZPY_RW() { ZPI_R(Y); }

//--------------------------- absolute addressing ------------------------------

static inline void ABS_W()
{
	EA = FETCH_PC16();
}

static inline void ABS_R()
{
	ABS_W();
	DT = READ8(EA);
}

static inline void ABS_RW()
{
	ABS_R();
	WRITE8(EA, DT);
}

//----------------------- absolute indexed addressing --------------------------

static inline void ABI_W(u8 reg)
{
	ET = FETCH_PC16();
	EA = ET + reg;
	READ8((EA&0x00FF) | (ET&0xFF00));
}

static inline void ABI_R(u8 reg)
{
	ET = FETCH_PC16();
	EA = ET + reg;
	CHECK_EA();
	DT = READ8(EA);
}

static inline void ABI_RW(u8 reg)
{
	ABI_W(reg);
	DT = READ8(EA);
	WRITE8(EA, DT);
}

static inline void ABX_W() { ABI_W(X); }
static inline void ABY_W() { ABI_W(Y); }

static inline void ABX_R() { ABI_R(X); }
static inline void ABY_R() { ABI_R(Y); }

static inline void ABX_RW() { ABI_RW(X); }
static inline void ABY_RW() { ABI_RW(Y); }

//----------------------- indexed indirect addressing --------------------------

static inline void IDX_W()
{
	DT = FETCH_PC8();
	EA = FETCH_ZP16(DT + X);
}

static inline void IDX_R()
{
	IDX_W();
	DT = READ8(EA);
}

static inline void IDX_RW()
{
	IDX_R();
	WRITE8(EA, DT);
}

static inline void IDY_W()
{
	DT = FETCH_PC8();
	ET = FETCH_ZP16(DT);
	EA = ET + Y;
	READ8((EA&0x00FF) | (ET&0xFF00));
}

static inline void IDY_R()
{
	DT = FETCH_PC8();
	ET = FETCH_ZP16(DT);
	EA = ET + Y;
	CHECK_EA();
	DT = READ8(EA);
}

static inline void IDY_RW()
{
	IDY_W();
	DT = READ8(EA);
	WRITE8(EA, DT);
}

//------------------------- accumulator addressing -----------------------------

static inline void ACC_RW()
{
	DT = A;
}

static inline void STORE_ACC()
{
	A = DT;
}

static inline void STORE_ZPG() { nesRam[EA&0xFF] = DT; }
static inline void STORE_ZPX() { STORE_ZPG(); }
static inline void STORE_ZPY() { STORE_ZPG(); }

static inline void STORE_MEM() { WRITE8(EA, DT); }
static inline void STORE_ABS() { STORE_MEM(); }
static inline void STORE_ABX() { STORE_MEM(); }
static inline void STORE_ABY() { STORE_MEM(); }
static inline void STORE_IDX() { STORE_MEM(); }
static inline void STORE_IDY() { STORE_MEM(); }

#define OP___(ci,op) case ci: op(); break
#define OP_R_(ci,op,addr) case ci: addr##_R(); op(); break
#define OP__W(ci,op,addr) case ci: addr##_W(); op(); STORE_##addr(); break
#define OP_RW(ci,op,addr) case ci: addr##_RW(); op(); STORE_##addr(); break

static void execute(u8 opcode)
{
	switch (opcode) {
	OP___(0x00, BRK     );
	OP_R_(0x01, ORA, IDX);
	OP___(0x02, KIL     );
	OP_RW(0x03, SLO, IDX);
	OP___(0x04, DOP     );
	OP_R_(0x05, ORA, ZPG);
	OP_RW(0x06, ASL, ZPG);
	OP_RW(0x07, SLO, ZPG);
	OP___(0x08, PHP     );
	OP_R_(0x09, ORA, IMM);
	OP_RW(0x0A, ASL, ACC);
	OP_R_(0x0B, AAC, IMM);
	OP_R_(0x0C, NOP, ABS);
	OP_R_(0x0D, ORA, ABS);
	OP_RW(0x0E, ASL, ABS);
	OP_RW(0x0F, SLO, ABS);
	OP_R_(0x10, BPL, IMM);
	OP_R_(0x11, ORA, IDY);
	OP___(0x12, KIL     );
	OP_RW(0x13, SLO, IDY);
	OP___(0x14, DOP     );
	OP_R_(0x15, ORA, ZPX);
	OP_RW(0x16, ASL, ZPX);
	OP_RW(0x17, SLO, ZPX);
	OP___(0x18, CLC     );
	OP_R_(0x19, ORA, ABY);
	OP___(0x1A, NOP     );
	OP_RW(0x1B, SLO, ABY);
	OP_R_(0x1C, NOP, ABX);
	OP_R_(0x1D, ORA, ABX);
	OP_RW(0x1E, ASL, ABX);
	OP_RW(0x1F, SLO, ABX);
	OP___(0x20, JSR     );
	OP_R_(0x21, AND, IDX);
	OP___(0x22, KIL     );
	OP_RW(0x23, RLA, IDX);
	OP_R_(0x24, BIT, ZPG);
	OP_R_(0x25, AND, ZPG);
	OP_RW(0x26, ROL, ZPG);
	OP_RW(0x27, RLA, ZPG);
	OP___(0x28, PLP     );
	OP_R_(0x29, AND, IMM);
	OP_RW(0x2A, ROL, ACC);
	OP_R_(0x2B, AAC, IMM);
	OP_R_(0x2C, BIT, ABS);
	OP_R_(0x2D, AND, ABS);
	OP_RW(0x2E, ROL, ABS);
	OP_RW(0x2F, RLA, ABS);
	OP_R_(0x30, BMI, IMM);
	OP_R_(0x31, AND, IDY);
	OP___(0x32, KIL     );
	OP_RW(0x33, RLA, IDY);
	OP___(0x34, DOP     );
	OP_R_(0x35, AND, ZPX);
	OP_RW(0x36, ROL, ZPX);
	OP_RW(0x37, RLA, ZPX);
	OP___(0x38, SEC     );
	OP_R_(0x39, AND, ABY);
	OP___(0x3A, NOP     );
	OP_RW(0x3B, RLA, ABY);
	OP_R_(0x3C, NOP, ABX);
	OP_R_(0x3D, AND, ABX);
	OP_RW(0x3E, ROL, ABX);
	OP_RW(0x3F, RLA, ABX);
	OP___(0x40, RTI     );
	OP_R_(0x41, EOR, IDX);
	OP___(0x42, KIL     );
	OP_RW(0x43, SRE, IDX);
	OP___(0x44, DOP     );
	OP_R_(0x45, EOR, ZPG);
	OP_RW(0x46, LSR, ZPG);
	OP_RW(0x47, SRE, ZPG);
	OP___(0x48, PHA     );
	OP_R_(0x49, EOR, IMM);
	OP_RW(0x4A, LSR, ACC);
	OP_R_(0x4B, ASR, IMM);
	OP___(0x4C, JMP_ABS );
	OP_R_(0x4D, EOR, ABS);
	OP_RW(0x4E, LSR, ABS);
	OP_RW(0x4F, SRE, ABS);
	OP_R_(0x50, BVC, IMM);
	OP_R_(0x51, EOR, IDY);
	OP___(0x52, KIL     );
	OP_RW(0x53, SRE, IDY);
	OP___(0x54, DOP     );
	OP_R_(0x55, EOR, ZPX);
	OP_RW(0x56, LSR, ZPX);
	OP_RW(0x57, SRE, ZPX);
	OP___(0x58, CLI     );
	OP_R_(0x59, EOR, ABY);
	OP___(0x5A, NOP     );
	OP_RW(0x5B, SRE, ABY);
	OP_R_(0x5C, NOP, ABX);
	OP_R_(0x5D, EOR, ABX);
	OP_RW(0x5E, LSR, ABX);
	OP_RW(0x5F, SRE, ABX);
	OP___(0x60, RTS     );
	OP_R_(0x61, ADC, IDX);
	OP___(0x62, KIL);
	OP_RW(0x63, RRA, IDX);
	OP___(0x64, DOP);
	OP_R_(0x65, ADC, ZPG);
	OP_RW(0x66, ROR, ZPG);
	OP_RW(0x67, RRA, ZPG);
	OP___(0x68, PLA     );
	OP_R_(0x69, ADC, IMM);
	OP_RW(0x6A, ROR, ACC);
	OP_R_(0x6B, ARR, IMM);
	OP___(0x6C, JMP_IND );
	OP_R_(0x6D, ADC, ABS);
	OP_RW(0x6E, ROR, ABS);
	OP_RW(0x6F, RRA, ABS);
	OP_R_(0x70, BVS, IMM);
	OP_R_(0x71, ADC, IDY);
	OP___(0x72, KIL     );
	OP_RW(0x73, RRA, IDY);
	OP___(0x74, DOP     );
	OP_R_(0x75, ADC, ZPX);
	OP_RW(0x76, ROR, ZPX);
	OP_RW(0x77, RRA, ZPX);
	OP___(0x78, SEI     );
	OP_R_(0x79, ADC, ABY);
	OP___(0x7A, NOP     );
	OP_RW(0x7B, RRA, ABY);
	OP_R_(0x7C, NOP, ABX);
	OP_R_(0x7D, ADC, ABX);
	OP_RW(0x7E, ROR, ABX);
	OP_RW(0x7F, RRA, ABX);
	OP___(0x80, DOP     );
	OP__W(0x81, STA, IDX);
	OP___(0x82, DOP     );
	OP__W(0x83, AAX, IDX);
	OP__W(0x84, STY, ZPG);
	OP__W(0x85, STA, ZPG);
	OP__W(0x86, STX, ZPG);
	OP__W(0x87, AAX, ZPG);
	OP___(0x88, DEY     );
	OP___(0x89, DOP     );
	OP___(0x8A, TXA     );
	OP_R_(0x8B, XAA, IMM);
	OP__W(0x8C, STY, ABS);
	OP__W(0x8D, STA, ABS);
	OP__W(0x8E, STX, ABS);
	OP__W(0x8F, AAX, ABS);
	OP_R_(0x90, BCC, IMM);
	OP__W(0x91, STA, IDY);
	OP___(0x92, KIL);
	OP__W(0x93, SHA, IDY);
	OP__W(0x94, STY, ZPX);
	OP__W(0x95, STA, ZPX);
	OP__W(0x96, STX, ZPY);
	OP__W(0x97, AAX, ZPY);
	OP___(0x98, TYA     );
	OP__W(0x99, STA, ABY);
	OP___(0x9A, TXS     );
	OP__W(0x9B, XAS, ABY);
	OP__W(0x9C, SYA, ABX);
	OP__W(0x9D, STA, ABX);
	OP__W(0x9E, SXA, ABY);
	OP__W(0x9F, SHA, ABY);
	OP_R_(0xA0, LDY, IMM);
	OP_R_(0xA1, LDA, IDX);
	OP_R_(0xA2, LDX, IMM);
	OP_R_(0xA3, LAX, IDX);
	OP_R_(0xA4, LDY, ZPG);
	OP_R_(0xA5, LDA, ZPG);
	OP_R_(0xA6, LDX, ZPG);
	OP_R_(0xA7, LAX, ZPG);
	OP___(0xA8, TAY     );
	OP_R_(0xA9, LDA, IMM);
	OP___(0xAA, TAX     );
	OP_R_(0xAB, ATX, IMM);
	OP_R_(0xAC, LDY, ABS);
	OP_R_(0xAD, LDA, ABS);
	OP_R_(0xAE, LDX, ABS);
	OP_R_(0xAF, LAX, ABS);
	OP_R_(0xB0, BCS, IMM);
	OP_R_(0xB1, LDA, IDY);
	OP___(0xB2, KIL     );
	OP_R_(0xB3, LAX, IDY);
	OP_R_(0xB4, LDY, ZPX);
	OP_R_(0xB5, LDA, ZPX);
	OP_R_(0xB6, LDX, ZPY);
	OP_R_(0xB7, LAX, ZPY);
	OP___(0xB8, CLV     );
	OP_R_(0xB9, LDA, ABY);
	OP___(0xBA, TSX     );
	OP_R_(0xBB, LAR, ABY);
	OP_R_(0xBC, LDY, ABX);
	OP_R_(0xBD, LDA, ABX);
	OP_R_(0xBE, LDX, ABY);
	OP_R_(0xBF, LAX, ABY);
	OP_R_(0xC0, CPY, IMM);
	OP_R_(0xC1, CMP, IDX);
	OP___(0xC2, DOP     );
	OP_RW(0xC3, DCP, IDX);
	OP_R_(0xC4, CPY, ZPG);
	OP_R_(0xC5, CMP, ZPG);
	OP_RW(0xC6, DEC, ZPG);
	OP_RW(0xC7, DCP, ZPG);
	OP___(0xC8, INY     );
	OP_R_(0xC9, CMP, IMM);
	OP___(0xCA, DEX     );
	OP_R_(0xCB, AXS, IMM);
	OP_R_(0xCC, CPY, ABS);
	OP_R_(0xCD, CMP, ABS);
	OP_RW(0xCE, DEC, ABS);
	OP_RW(0xCF, DCP, ABS);
	OP_R_(0xD0, BNE, IMM);
	OP_R_(0xD1, CMP, IDY);
	OP___(0xD2, KIL     );
	OP_RW(0xD3, DCP, IDY);
	OP___(0xD4, DOP     );
	OP_R_(0xD5, CMP, ZPX);
	OP_RW(0xD6, DEC, ZPX);
	OP_RW(0xD7, DCP, ZPX);
	OP___(0xD8, CLD     );
	OP_R_(0xD9, CMP, ABY);
	OP___(0xDA, NOP     );
	OP_RW(0xDB, DCP, ABY);
	OP_R_(0xDC, NOP, ABX);
	OP_R_(0xDD, CMP, ABX);
	OP_RW(0xDE, DEC, ABX);
	OP_RW(0xDF, DCP, ABX);
	OP_R_(0xE0, CPX, IMM);
	OP_R_(0xE1, SBC, IDX);
	OP___(0xE2, DOP     );
	OP_RW(0xE3, ISB, IDX);
	OP_R_(0xE4, CPX, ZPG);
	OP_R_(0xE5, SBC, ZPG);
	OP_RW(0xE6, INC, ZPG);
	OP_RW(0xE7, ISB, ZPG);
	OP___(0xE8, INX     );
	OP_R_(0xE9, SBC, IMM);
	OP___(0xEA, NOP     );
	OP_R_(0xEB, SBC, IMM);
	OP_R_(0xEC, CPX, ABS);
	OP_R_(0xED, SBC, ABS);
	OP_RW(0xEE, INC, ABS);
	OP_RW(0xEF, ISB, ABS);
	OP_R_(0xF0, BEQ, IMM);
	OP_R_(0xF1, SBC, IDY);
	OP___(0xF2, KIL     );
	OP_RW(0xF3, ISB, IDY);
	OP___(0xF4, DOP     );
	OP_R_(0xF5, SBC, ZPX);
	OP_RW(0xF6, INC, ZPX);
	OP_RW(0xF7, ISB, ZPX);
	OP___(0xF8, SED     );
	OP_R_(0xF9, SBC, ABY);
	OP___(0xFA, NOP     );
	OP_RW(0xFB, ISB, ABY);
	OP_R_(0xFC, NOP, ABX);
	OP_R_(0xFD, SBC, ABX);
	OP_RW(0xFE, INC, ABX);
	OP_RW(0xFF, ISB, ABX);
	}
}

const u8 NesCpu::cyclesTable[256] =
{
/* 0x00 */7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
/* 0x10 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0x20 */6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
/* 0x30 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0x40 */6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
/* 0x50 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0x60 */6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
/* 0x70 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0x80 */2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
/* 0x90 */2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
/* 0xA0 */2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
/* 0xB0 */2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
/* 0xC0 */2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
/* 0xD0 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 0xE0 */2, 6, 3, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
/* 0xF0 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7
};

const u8 NesCpu::sizeTable[256] =
{
/* 0x00 */1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x10 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0x20 */3, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x30 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0x40 */1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x50 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0x60 */1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x70 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0x80 */2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0x90 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0xA0 */2, 2, 2, 2, 2, 2, 2, 3, 1, 2, 1, 2, 3, 3, 3, 4,
/* 0xB0 */2, 2, 1, 2, 2, 2, 2, 4, 1, 3, 1, 3, 3, 3, 3, 4,
/* 0xC0 */2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0xD0 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
/* 0xE0 */2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
/* 0xF0 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3
};

const u8 NesCpu::addressingModeTable[256] =
{
/* 0x00 */Impli, IndiX, Impli, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x08 */Impli, Immed, Accum, Immed, Absol, Absol, Absol, Absol,
/* 0x10 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0x18 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0x20 */Absol, IndiX, Impli, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x28 */Impli, Immed, Accum, Immed, Absol, Absol, Absol, Absol,
/* 0x30 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0x38 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0x40 */Impli, IndiX, Impli, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x48 */Impli, Immed, Accum, Immed, Absol, Absol, Absol, Absol,
/* 0x50 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0x58 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0x60 */Impli, IndiX, Impli, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x68 */Impli, Immed, Accum, Immed, Indir, Absol, Absol, Absol,
/* 0x70 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0x78 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0x80 */Immed, IndiX, Immed, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0x88 */Impli, Immed, Impli, Immed, Absol, Absol, Absol, Absol,
/* 0x90 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPY, ZerPY,
/* 0x98 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoY, AbsoY,
/* 0xA0 */Immed, IndiX, Immed, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0xA8 */Impli, Immed, Impli, Immed, Absol, Absol, Absol, Absol,
/* 0xB0 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPY, ZerPY,
/* 0xB8 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoY, AbsoY,
/* 0xC0 */Immed, IndiX, Immed, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0xC8 */Impli, Immed, Impli, Immed, Absol, Absol, Absol, Absol,
/* 0xD0 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0xD8 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX,
/* 0xE0 */Immed, IndiX, Immed, IndiX, ZeroP, ZeroP, ZeroP, ZeroP,
/* 0xE8 */Impli, Immed, Impli, Immed, Absol, Absol, Absol, Absol,
/* 0xF0 */Relat, IndiY, Impli, IndiY, ZerPX, ZerPX, ZerPX, ZerPX,
/* 0xF8 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX
};

const char *NesCpu::nameTable[256] =
{
/* 0x00 */"BRK", "ORA", "KIL", "SLO", "DOP", "ORA", "ASL", "SLO",
/* 0x08 */"PHP", "ORA", "ASL", "AAC", "TOP", "ORA", "ASL", "SLO",
/* 0x10 */"BPL", "ORA", "KIL", "SLO", "DOP", "ORA", "ASL", "SLO",
/* 0x18 */"CLC", "ORA", "NOP", "SLO", "TOP", "ORA", "ASL", "SLO",
/* 0x20 */"JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
/* 0x28 */"PLP", "AND", "ROL", "AAC", "BIT", "AND", "ROL", "RLA",
/* 0x30 */"BMI", "AND", "KIL", "RLA", "DOP", "AND", "ROL", "RLA",
/* 0x38 */"SEC", "AND", "NOP", "RLA", "TOP", "AND", "ROL", "RLA",
/* 0x40 */"RTI", "EOR", "KIL", "SRE", "DOP", "EOR", "LSR", "SRE",
/* 0x48 */"PHA", "EOR", "LSR", "ASR", "JMP", "EOR", "LSR", "SRE",
/* 0x50 */"BVC", "EOR", "KIL", "SRE", "DOP", "EOR", "LSR", "SRE",
/* 0x58 */"CLI", "EOR", "NOP", "SRE", "TOP", "EOR", "LSR", "SRE",
/* 0x60 */"RTS", "ADC", "KIL", "RRA", "DOP", "ADC", "ROR", "RRA",
/* 0x68 */"PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
/* 0x70 */"BVS", "ADC", "KIL", "RRA", "DOP", "ADC", "ROR", "RRA",
/* 0x78 */"SEI", "ADC", "NOP", "RRA", "TOP", "ADC", "ROR", "RRA",
/* 0x80 */"DOP", "STA", "DOP", "AAX", "STY", "STA", "STX", "AAX",
/* 0x88 */"DEY", "DOP", "TXA", "XAA", "STY", "STA", "STX", "AAX",
/* 0x90 */"BCC", "STA", "KIL", "AXA", "STY", "STA", "STX", "AAX",
/* 0x98 */"TYA", "STA", "TXS", "XAS", "SYA", "STA", "SXA", "AXA",
/* 0xA0 */"LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
/* 0xA8 */"TAY", "LDA", "TAX", "ATX", "LDY", "LDA", "LDX", "LAX",
/* 0xB0 */"BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
/* 0xB8 */"CLV", "LDA", "TSX", "LAR", "LDY", "LDA", "LDX", "LAX",
/* 0xC0 */"CPY", "CMP", "DOP", "DCP", "CPY", "CMP", "DEC", "DCP",
/* 0xC8 */"INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
/* 0xD0 */"BNE", "CMP", "KIL", "DCP", "DOP", "CMP", "DEC", "DCP",
/* 0xD8 */"CLD", "CMP", "NOP", "DCP", "TOP", "CMP", "DEC", "DCP",
/* 0xE0 */"CPX", "SBC", "DOP", "ISC", "CPX", "SBC", "INC", "ISC",
/* 0xE8 */"INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
/* 0xF0 */"BEQ", "SBC", "KIL", "ISC", "DOP", "SBC", "INC", "ISC",
/* 0xF8 */"SED", "SBC", "NOP", "ISC", "TOP", "SBC", "INC", "ISC"
};
