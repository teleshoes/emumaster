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
#include "machine.h"
#include "mapper.h"
#include "apu.h"

#include <imachine.h>
#include <QDataStream>

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

NesCpu nesCpu;

static const int StackBase = 0x100;

#define PUSH(data) WRITE(StackBase+S, data); S--
#define POP() READ(StackBase + (++S))

static u8 A; // accumulator register
static u8 Y, X; // index registers
static u16 PC; // program counter register
static u8 S; // stack pointer register
static u8 P; // processor status register

static u8 ZNTable[256];

static u32 cpuSignals;
static bool nmiState;
static u32 currentCycles;

static u32 dmaCycles;

static bool apuIrq;
static bool mapperIrq;

void NesCpu::init() {
	A = 0;
	Y = 0;
	X = 0;
	PC = 0;
	S = 0xFF;
	P = 0;
	nmiState = false;
	cpuSignals = Reset;
	qMemSet(ZNTable + 0x00, 0, 0x80);
	qMemSet(ZNTable + 0x80, N, 0x80);
	ZNTable[0] = Z;
}

u32 NesCpu::clock(u32 cycles) {
	u32 executedCycles = 0;
	if (cpuSignals & Reset) {
		apuIrq = false;
		mapperIrq = false;
		irq0_i(false);

		while (executedCycles < cycles)
			executedCycles += executeOne();
		nesApu.reset();
		dmaCycles = 0;
	} else {
		if (dmaCycles) {
			if (cycles <= dmaCycles) {
				dmaCycles -= cycles;
				nesMapper->clock(cycles);
				return cycles;
			} else {
				executedCycles += dmaCycles;
				dmaCycles = 0;
				nesMapper->clock(executedCycles);
			}
		}
		while (executedCycles < cycles) {
			u32 instrCycles = executeOne();
			// TODO mapper clock enable
			nesMapper->clock(instrCycles);
			executedCycles += instrCycles;
			nesApu.clockFrameCounter(instrCycles);
		}
//		TODO nesApu.clockFrameCounter(executedCycles);
	}
	return executedCycles;
}

void NesCpu::dma(u32 cycles)
{ dmaCycles += cycles; }

void NesCpu::apu_irq_i(bool on) {
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

// TODO remove log
//#include <QFile>
//static  QFile cpuLog("cpu.log");
//	cpuLog.open(QIODevice::ReadWrite|QIODevice::Truncate);

u32 NesCpu::executeOne() {
	currentCycles = 0;
	u8 opcode = READ(PC);
	ADDCYC(cyclesTable[opcode]);
	PC++;

//	cpuLog.write(qPrintable(QString::number(opcode, 16)));

//	cpuLog.write(" PC=");
//	cpuLog.write(qPrintable(QString::number(PC, 16)));
//	cpuLog.write(" A=");
//	cpuLog.write(qPrintable(QString::number(A, 16)));
//	cpuLog.write(" Y=");
//	cpuLog.write(qPrintable(QString::number(Y, 16)));
//	cpuLog.write(" X=");
//	cpuLog.write(qPrintable(QString::number(X, 16)));
//	cpuLog.write(" S=");
//	cpuLog.write(qPrintable(QString::number(S, 16)));

//	cpuLog.write(" [S+1]=");
//	cpuLog.write(qPrintable(QString::number(READ(S+0x101), 16)));
//	cpuLog.write(" [S+2]=");
//	cpuLog.write(qPrintable(QString::number(READ(S+0x102), 16)));
//	cpuLog.write(" [S+3]=");
//	cpuLog.write(qPrintable(QString::number(READ(S+0x103), 16)));

//	cpuLog.write(" [PC+1]=");
//	cpuLog.write(qPrintable(QString::number(READ(PC+0), 16)));
//	cpuLog.write(" [PC+2]=");
//	cpuLog.write(qPrintable(QString::number(READ(PC+1), 16)));
//	cpuLog.write(" [PC+3]=");
//	cpuLog.write(qPrintable(QString::number(READ(PC+2), 16)));

	execute(opcode);

//	cpuLog.write(" PCa=");
//	cpuLog.write(qPrintable(QString::number(PC, 16)));
//	cpuLog.write("\n");

	if (cpuSignals) {
		if ((cpuSignals & (Reset | Nmi)) || !(P & I)) {
			u16 vector;
			if (cpuSignals & Reset) {
				vector = ResetVectorAddress;
				P = I;
				A = 0;
				Y = 0;
				X = 0;
				S = 0xFF;
			} else if (cpuSignals & Nmi) {
				vector = NmiVectorAddress;
				cpuSignals &= ~Nmi;
//				cpuLog.write("nmi\n");
			} else {
				vector = IrqVectorAddress;
//				cpuLog.write("irq\n");
			}
			ADDCYC(7);
			PUSH(PC >> 8);
			PUSH(PC);
			PUSH((P & ~B) | U);
			P |= I;
			PC = READ(vector);
			PC |= READ(vector + 1) << 8;
		}
	}
	return currentCycles;
}

void NesCpu::ADDCYC(u32 n)
{ currentCycles += n; }

void NesCpu::setSignal(SignalIn sig, bool on) {
	if (on)
		cpuSignals |=  sig;
	else
		cpuSignals &= ~sig;
}

void NesCpu::irq0_i(bool on)
{ setSignal(Irq0, on); }

void NesCpu::nmi_i(bool on) {
	if (on && !nmiState)
		setSignal(Nmi, true);
	nmiState = on;
}
void NesCpu::reset_i(bool on)
{ setSignal(Reset, on); }

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(NesCpu, 1) \
	STATE_SERIALIZE_VAR_##sl(dmaCycles) \
	STATE_SERIALIZE_VAR_##sl(apuIrq) \
	STATE_SERIALIZE_VAR_##sl(mapperIrq) \
	STATE_SERIALIZE_VAR_##sl(cpuSignals) \
	STATE_SERIALIZE_VAR_##sl(nmiState) \
	STATE_SERIALIZE_VAR_##sl(A) \
	STATE_SERIALIZE_VAR_##sl(Y) \
	STATE_SERIALIZE_VAR_##sl(X) \
	STATE_SERIALIZE_VAR_##sl(PC) \
	STATE_SERIALIZE_VAR_##sl(S) \
	STATE_SERIALIZE_VAR_##sl(P) \
STATE_SERIALIZE_END_##sl(NesCpu)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)

#define X_ZN(val)	P &= ~(Z|N); P |= ZNTable[val]
#define X_ZNT(val)	P |= ZNTable[val]

#define JR(cond) { \
	if (cond) { \
		u32 tmp; \
		s32 disp = static_cast<s8> (READ(PC)); \
		PC++; \
		ADDCYC(1); \
		tmp = PC; \
		PC += disp; \
		if ((tmp^PC) & 0x100) \
			ADDCYC(1); \
	} else { \
		PC++; \
	} \
}

#define LDA  A = x; X_ZN(A)
#define LDX  X = x; X_ZN(X)
#define LDY  Y = x; X_ZN(Y)

#define AND  A &= x; X_ZN(A)
#define BIT  P &= ~(Z|V|N); P |= ZNTable[x&A]&Z; P |= x&(V|N)
#define EOR  A ^= x; X_ZN(A)
#define ORA  A |= x; X_ZN(A)

#define ADC { \
	u32 l = A+x+(P&1); \
	P &= ~(Z|C|N|V); \
	P |= ((((A^x)&0x80)^0x80) & ((A^l)&0x80)) >> 1; \
	P |= (l>>8)&C;  \
	A = l; \
	X_ZNT(A); \
}

#define SBC { \
	u32 l = A-x-((P&1)^1); \
	P &= ~(Z|C|N|V); \
	P |= ((A^l) & (A^x)&0x80) >> 1; \
	P |= ((l>>8)&C) ^ C; \
	A = l; \
	X_ZNT(A); \
}

#define CMPL(a1,a2) { \
	u32 t = a1-a2; \
	X_ZN(t & 0xFF); \
	P &= ~C; \
	P |= ((t>>8)&C) ^ C; \
}

#define AXS { \
	u32 t = (A&X) - x;    \
	X_ZN(t & 0xFF); \
	P &= ~C; \
	P |= ((t>>8)&C) ^ C; \
	X = t; \
}

#define CMP CMPL(A, x)
#define CPX CMPL(X, x)
#define CPY CMPL(Y, x)

#define DEC x--; X_ZN(x)
#define INC x++; X_ZN(x)

#define ASL P &= ~C; P |= x>>7; x<<=1; X_ZN(x)
#define LSR P &= ~(C|N|Z); P |= x&1; x>>=1; X_ZNT(x)

#define LSRA P &= ~(C|N|Z); P |= A&1; A>>=1; X_ZNT(A)

#define ROL { \
	u8 l = x>>7; \
	x <<= 1; \
	x |= P&C; \
	P &= ~(Z|N|C); \
	P |= l; \
	X_ZNT(x); \
}
#define ROR { \
	u8 l = x&1; \
	x >>= 1; \
	x |= (P&C) << 7; \
	P &= ~(Z|N|C); \
	P |= l; \
	X_ZNT(x); \
}

#define RMW_A(op)	{ u8 x = A; op; A=x; }
#define RMW_AB(op)	{ u32 a; getAB(a); u8 x = READ(a); WRITE(a,x); op; WRITE(a,x); }
#define RMW_ABI(reg, op) { u32 a; getABIWR(a,reg); u8 x = READ(a); WRITE(a,x); op; WRITE(a,x); }
#define RMW_ABX(op)	RMW_ABI(X, op)
#define RMW_ABY(op)	RMW_ABI(Y, op)
#define RMW_IX(op)  { u32 a; getIX(a); u8 x = READ(a); WRITE(a,x); op; WRITE(a,x); }
#define RMW_IY(op)  { u32 a; getIYWR(a); u8 x = READ(a); WRITE(a,x); op; WRITE(a,x); }
#define RMW_ZP(op)  { u8 a = getZP(); u8 x = READ(a); op; WRITE(a,x); }
#define RMW_ZPX(op) { u8 a = getZPI(X); u8 x = READ(a); op; WRITE(a,x); }

#define LD_IM(op)	{ u8 x = READ(PC); PC++; op; }
#define LD_ZP(op)	{ u8 a = getZP(); u8 x = READ(a); op; }
#define LD_ZPX(op)	{ u8 a = getZPI(X); u8 x = READ(a); op; }
#define LD_ZPY(op)	{ u8 a = getZPI(Y); u8 x = READ(a); op; }
#define LD_AB(op)	{ u32 a; getAB(a); u8 x = READ(a); op; }
#define LD_ABI(reg,op)	{ u32 a; getABIRD(a,reg); u8 x = READ(a); op; }
#define LD_ABX(op)	LD_ABI(X,op)
#define LD_ABY(op)	LD_ABI(Y,op)
#define LD_IX(op)	{ u32 a; getIX(a); u8 x = READ(a); op; }
#define LD_IY(op)	{ u32 a; getIYRD(a); u8 x = READ(a); op; }

#define ST_ZP(r)	{ u8 a = getZP(); WRITE(a,r); }
#define ST_ZPX(r)	{ u8 a = getZPI(X); WRITE(a,r); }
#define ST_ZPY(r)	{ u8 a = getZPI(Y); WRITE(a,r); }
#define ST_AB(r)	{ u32 a; getAB(a); WRITE(a,r); }
#define ST_ABI(reg,r)	{ u32 a; getABIWR(a,reg); WRITE(a,r); }
#define ST_ABX(r)	ST_ABI(X, r)
#define ST_ABY(r)	ST_ABI(Y, r)
#define ST_IX(r)	{ u32 a; getIX(a); WRITE(a,r); }
#define ST_IY(r)	{ u32 a; getIYWR(a); WRITE(a,r); }

#define getAB(target) { \
	target = READ(PC++); \
	target |= READ(PC++)<<8; \
}

#define getABIRD(target,i) { \
	u32 tmp; \
	getAB(tmp); \
	target = tmp; \
	target += i; \
	if ((target^tmp) & 0x100) { \
		target &= 0xFFFF; \
		READ(target ^ 0x100); \
		ADDCYC(1); \
	} \
}

#define getABIWR(target,i) { \
	u32 rt; \
	getAB(rt); \
	target = rt; \
	target += i; \
	target &= 0xFFFF; \
	READ((target&0x00FF) | (rt&0xFF00)); \
}

#define getZP() READ(PC++)
#define getZPI(i) i+READ(PC++)

#define getIX(target) { \
	u8 tmp = READ(PC++); \
	tmp += X; \
	target = READ(tmp++); \
	target |= READ(tmp) << 8; \
}

#define getIYRD(target) { \
	u8 tmp = READ(PC++); \
	u32 rt = READ(tmp++); \
	rt |= READ(tmp) << 8; \
	target = rt + Y; \
	if ((target^rt) & 0x100) { \
		target &= 0xFFFF; \
		READ(target ^ 0x100); \
		ADDCYC(1); \
	} \
}

#define getIYWR(target) { \
	u8 tmp = READ(PC++); \
	uint rt = READ(tmp++); \
	rt |= READ(tmp) << 8; \
	target = (rt + Y) & 0xFFFF; \
	READ((target&0x00FF) | (rt&0xFF00)); \
}

#define BRK \
	PC++; \
	PUSH(PC >> 8); \
	PUSH(PC); \
	PUSH(P|U|B); \
	P |= I; \
	PC = READ(0xFFFE); \
	PC |= READ(0xFFFF) << 8;

#define RTI \
	P = POP(); \
	PC = POP(); \
	PC |= POP() << 8;

#define RTS \
	PC = POP(); \
	PC |= POP() << 8; \
	PC++;

#define PHA PUSH(A)
#define PHP PUSH(P|U|B)
#define PLA A = POP(); X_ZN(A)
#define PLP P = POP()

#define JMP_ABS { \
	u32 npc = READ(PC); \
	npc |= READ(PC+1) << 8; \
	PC = npc; \
}

#define JMP_IND { \
	u32 tmp; \
	getAB(tmp); \
	PC = READ(tmp); \
	PC |= READ(((tmp+1)&0x00FF) | (tmp&0xFF00)) << 8; \
}

#define JSR { \
	u8 npc = READ(PC++); \
	PUSH(PC >> 8); \
	PUSH(PC); \
	PC = READ(PC) << 8; \
	PC |= npc; \
}

#define TAX X = A; X_ZN(A)
#define TXA A = X; X_ZN(A)
#define TAY Y = A; X_ZN(A)
#define TYA A = Y; X_ZN(A)
#define TSX X = S; X_ZN(X)
#define TXS S = X
#define DEX X--; X_ZN(X)
#define DEY Y--; X_ZN(Y)
#define INX X++; X_ZN(X)
#define INY Y++; X_ZN(Y)
#define CLC P &= ~C
#define CLD P &= ~D
#define CLI P &= ~I
#define CLV P &= ~V
#define SEC P |= C
#define SED P |= D
#define SEI P |= I
#define NOP

#define BCC JR(!(P&C))
#define BCS JR(P&C)
#define BEQ JR(P&Z)
#define BNE JR(!(P&Z))
#define BMI JR(P&N)
#define BPL JR(!(P&N))
#define BVC JR(!(P&V))
#define BVS JR(P&V)

#define AAC AND;P &= ~C; P |= A>>7
#define AAX A&X

#define ARR {\
	AND; \
	P &= ~V; \
	P |= (A^(A>>1)) & 0x40; \
	u8 arrtmp = A >> 7; \
	A >>= 1; \
	A |= (P&C) << 7; \
	P &= ~C; \
	P |= arrtmp; \
	X_ZN(A); \
}

#define ASR AND;LSRA
#define ATX A |= 0xFF; AND; X = A
#define DCP DEC;CMP
#define ISB INC;SBC
#define DOP PC++;
#define KIL ADDCYC(0xFF); PC--; // m_jammed = true;
#define LAR S &= x; A = X = S; X_ZN(X)
#define LAX LDA;LDX
#define RLA ROL;AND
#define RRA ROR;ADC
#define SHA A&X & (((a-Y)>>8)+1)
#define SLO ASL;ORA
#define SRE LSR;EOR
#define SYA Y & (((a-X)>>8)+1)
#define SXA X & (((a-Y)>>8)+1)
#define XAA A |= 0xEE; A &= X; LD_IM(AND);
#define XAS S = A & X; ST_ABY(S & (((a-Y)>>8)+1))

#define OP(ci,op) case ci: op; break

void NesCpu::execute(u8 instr) {
	switch (instr) {
	OP(0x00, BRK);
	OP(0x01, LD_IX(ORA));
	OP(0x02, KIL);
	OP(0x03, RMW_IX(SLO));
	OP(0x04, DOP);
	OP(0x05, LD_ZP(ORA));
	OP(0x06, RMW_ZP(ASL));
	OP(0x07, RMW_ZP(SLO));
	OP(0x08, PHP);
	OP(0x09, LD_IM(ORA));
	OP(0x0A, RMW_A(ASL));
	OP(0x0B, LD_IM(AAC));
	OP(0x0C, LD_AB(Q_UNUSED(x)));
	OP(0x0D, LD_AB(ORA));
	OP(0x0E, RMW_AB(ASL));
	OP(0x0F, RMW_AB(SLO));
	OP(0x10, BPL);
	OP(0x11, LD_IY(ORA));
	OP(0x12, KIL);
	OP(0x13, RMW_IY(SLO));
	OP(0x14, DOP);
	OP(0x15, LD_ZPX(ORA));
	OP(0x16, RMW_ZPX(ASL));
	OP(0x17, RMW_ZPX(SLO));
	OP(0x18, CLC);
	OP(0x19, LD_ABY(ORA));
	OP(0x1A, NOP);
	OP(0x1B, RMW_ABY(SLO));
	OP(0x1C, LD_ABX(Q_UNUSED(x)));
	OP(0x1D, LD_ABX(ORA));
	OP(0x1E, RMW_ABX(ASL));
	OP(0x1F, RMW_ABX(SLO));
	OP(0x20, JSR);
	OP(0x21, LD_IX(AND));
	OP(0x22, KIL);
	OP(0x23, RMW_IX(RLA));
	OP(0x24, LD_ZP(BIT));
	OP(0x25, LD_ZP(AND));
	OP(0x26, RMW_ZP(ROL));
	OP(0x27, RMW_ZP(RLA));
	OP(0x28, PLP);
	OP(0x29, LD_IM(AND));
	OP(0x2A, RMW_A(ROL));
	OP(0x2B, LD_IM(AAC));
	OP(0x2C, LD_AB(BIT));
	OP(0x2D, LD_AB(AND));
	OP(0x2E, RMW_AB(ROL));
	OP(0x2F, RMW_AB(RLA));
	OP(0x30, BMI);
	OP(0x31, LD_IY(AND));
	OP(0x32, KIL);
	OP(0x33, RMW_IY(RLA));
	OP(0x34, DOP);
	OP(0x35, LD_ZPX(AND));
	OP(0x36, RMW_ZPX(ROL));
	OP(0x37, RMW_ZPX(RLA));
	OP(0x38, SEC);
	OP(0x39, LD_ABY(AND));
	OP(0x3A, NOP);
	OP(0x3B, RMW_ABY(RLA));
	OP(0x3C, LD_ABX(Q_UNUSED(x)));
	OP(0x3D, LD_ABX(AND));
	OP(0x3E, RMW_ABX(ROL));
	OP(0x3F, RMW_ABX(RLA));
	OP(0x40, RTI);
	OP(0x41, LD_IX(EOR));
	OP(0x42, KIL);
	OP(0x43, RMW_IX(SRE));
	OP(0x44, DOP);
	OP(0x45, LD_ZP(EOR));
	OP(0x46, RMW_ZP(LSR));
	OP(0x47, RMW_ZP(SRE));
	OP(0x48, PHA);
	OP(0x49, LD_IM(EOR));
	OP(0x4A, RMW_A(LSR));
	OP(0x4B, LD_IM(ASR));
	OP(0x4C, JMP_ABS);
	OP(0x4D, LD_AB(EOR));
	OP(0x4E, RMW_AB(LSR));
	OP(0x4F, RMW_AB(SRE));
	OP(0x50, BVC);
	OP(0x51, LD_IY(EOR));
	OP(0x52, KIL);
	OP(0x53, RMW_IY(SRE));
	OP(0x54, DOP);
	OP(0x55, LD_ZPX(EOR));
	OP(0x56, RMW_ZPX(LSR));
	OP(0x57, RMW_ZPX(SRE));
	OP(0x58, CLI);
	OP(0x59, LD_ABY(EOR));
	OP(0x5A, NOP);
	OP(0x5B, RMW_ABY(SRE));
	OP(0x5C, LD_ABX(Q_UNUSED(x)));
	OP(0x5D, LD_ABX(EOR));
	OP(0x5E, RMW_ABX(LSR));
	OP(0x5F, RMW_ABX(SRE));
	OP(0x60, RTS);
	OP(0x61, LD_IX(ADC));
	OP(0x62, KIL);
	OP(0x63, RMW_IX(RRA));
	OP(0x64, DOP);
	OP(0x65, LD_ZP(ADC));
	OP(0x66, RMW_ZP(ROR));
	OP(0x67, RMW_ZP(RRA));
	OP(0x68, PLA);
	OP(0x69, LD_IM(ADC));
	OP(0x6A, RMW_A(ROR));
	OP(0x6B, LD_IM(ARR));
	OP(0x6C, JMP_IND);
	OP(0x6D, LD_AB(ADC));
	OP(0x6E, RMW_AB(ROR));
	OP(0x6F, RMW_AB(RRA));
	OP(0x70, BVS);
	OP(0x71, LD_IY(ADC));
	OP(0x72, KIL);
	OP(0x73, RMW_IY(RRA));
	OP(0x74, DOP);
	OP(0x75, LD_ZPX(ADC));
	OP(0x76, RMW_ZPX(ROR));
	OP(0x77, RMW_ZPX(RRA));
	OP(0x78, SEI);
	OP(0x79, LD_ABY(ADC));
	OP(0x7A, NOP);
	OP(0x7B, RMW_ABY(RRA));
	OP(0x7C, LD_ABX(Q_UNUSED(x)));
	OP(0x7D, LD_ABX(ADC));
	OP(0x7E, RMW_ABX(ROR));
	OP(0x7F, RMW_ABX(RRA));
	OP(0x80, DOP);
	OP(0x81, ST_IX(A));
	OP(0x82, DOP);
	OP(0x83, ST_IX(AAX));
	OP(0x84, ST_ZP(Y));
	OP(0x85, ST_ZP(A));
	OP(0x86, ST_ZP(X));
	OP(0x87, ST_ZP(AAX));
	OP(0x88, DEY);
	OP(0x89, DOP);
	OP(0x8A, TXA);
	OP(0x8B, XAA);
	OP(0x8C, ST_AB(Y));
	OP(0x8D, ST_AB(A));
	OP(0x8E, ST_AB(X));
	OP(0x8F, ST_AB(AAX));
	OP(0x90, BCC);
	OP(0x91, ST_IY(A));
	OP(0x92, KIL);
	OP(0x93, ST_IY(SHA));
	OP(0x94, ST_ZPX(Y));
	OP(0x95, ST_ZPX(A));
	OP(0x96, ST_ZPY(X));
	OP(0x97, ST_ZPY(AAX));
	OP(0x98, TYA);
	OP(0x99, ST_ABY(A));
	OP(0x9A, TXS);
	OP(0x9B, XAS);
	OP(0x9C, ST_ABX(SYA));
	OP(0x9D, ST_ABX(A));
	OP(0x9E, ST_ABY(SXA));
	OP(0x9F, ST_ABY(SHA));
	OP(0xA0, LD_IM(LDY));
	OP(0xA1, LD_IX(LDA));
	OP(0xA2, LD_IM(LDX));
	OP(0xA3, LD_IX(LAX));
	OP(0xA4, LD_ZP(LDY));
	OP(0xA5, LD_ZP(LDA));
	OP(0xA6, LD_ZP(LDX));
	OP(0xA7, LD_ZP(LAX));
	OP(0xA8, TAY);
	OP(0xA9, LD_IM(LDA));
	OP(0xAA, TAX);
	OP(0xAB, LD_IM(ATX));
	OP(0xAC, LD_AB(LDY));
	OP(0xAD, LD_AB(LDA));
	OP(0xAE, LD_AB(LDX));
	OP(0xAF, LD_AB(LAX));
	OP(0xB0, BCS);
	OP(0xB1, LD_IY(LDA));
	OP(0xB2, KIL);
	OP(0xB3, LD_IY(LAX));
	OP(0xB4, LD_ZPX(LDY));
	OP(0xB5, LD_ZPX(LDA));
	OP(0xB6, LD_ZPY(LDX));
	OP(0xB7, LD_ZPY(LAX));
	OP(0xB8, CLV);
	OP(0xB9, LD_ABY(LDA));
	OP(0xBA, TSX);
	OP(0xBB, RMW_ABY(LAR));
	OP(0xBC, LD_ABX(LDY));
	OP(0xBD, LD_ABX(LDA));
	OP(0xBE, LD_ABY(LDX));
	OP(0xBF, LD_ABY(LAX));
	OP(0xC0, LD_IM(CPY));
	OP(0xC1, LD_IX(CMP));
	OP(0xC2, DOP);
	OP(0xC3, RMW_IX(DCP));
	OP(0xC4, LD_ZP(CPY));
	OP(0xC5, LD_ZP(CMP));
	OP(0xC6, RMW_ZP(DEC));
	OP(0xC7, RMW_ZP(DCP));
	OP(0xC8, INY);
	OP(0xC9, LD_IM(CMP));
	OP(0xCA, DEX);
	OP(0xCB, LD_IM(AXS));
	OP(0xCC, LD_AB(CPY));
	OP(0xCD, LD_AB(CMP));
	OP(0xCE, RMW_AB(DEC));
	OP(0xCF, RMW_AB(DCP));
	OP(0xD0, BNE);
	OP(0xD1, LD_IY(CMP));
	OP(0xD2, KIL);
	OP(0xD3, RMW_IY(DCP));
	OP(0xD4, DOP);
	OP(0xD5, LD_ZPX(CMP));
	OP(0xD6, RMW_ZPX(DEC));
	OP(0xD7, RMW_ZPX(DCP));
	OP(0xD8, CLD);
	OP(0xD9, LD_ABY(CMP));
	OP(0xDA, NOP);
	OP(0xDB, RMW_ABY(DCP));
	OP(0xDC, LD_ABX(Q_UNUSED(x)));
	OP(0xDD, LD_ABX(CMP));
	OP(0xDE, RMW_ABX(DEC));
	OP(0xDF, RMW_ABX(DCP));
	OP(0xE0, LD_IM(CPX));
	OP(0xE1, LD_IX(SBC));
	OP(0xE2, DOP);
	OP(0xE3, RMW_IX(ISB));
	OP(0xE4, LD_ZP(CPX));
	OP(0xE5, LD_ZP(SBC));
	OP(0xE6, RMW_ZP(INC));
	OP(0xE7, RMW_ZP(ISB));
	OP(0xE8, INX);
	OP(0xE9, LD_IM(SBC));
	OP(0xEA, NOP);
	OP(0xEB, LD_IM(SBC));
	OP(0xEC, LD_AB(CPX));
	OP(0xED, LD_AB(SBC));
	OP(0xEE, RMW_AB(INC));
	OP(0xEF, RMW_AB(ISB));
	OP(0xF0, BEQ);
	OP(0xF1, LD_IY(SBC));
	OP(0xF2, KIL);
	OP(0xF3, RMW_IY(ISB));
	OP(0xF4, DOP);
	OP(0xF5, LD_ZPX(SBC));
	OP(0xF6, RMW_ZPX(INC));
	OP(0xF7, RMW_ZPX(ISB));
	OP(0xF8, SED);
	OP(0xF9, LD_ABY(SBC));
	OP(0xFA, NOP);
	OP(0xFB, RMW_ABY(ISB));
	OP(0xFC, LD_ABX(Q_UNUSED(x)));
	OP(0xFD, LD_ABX(SBC));
	OP(0xFE, RMW_ABX(INC));
	OP(0xFF, RMW_ABX(ISB));
	}
}

const u8 NesCpu::cyclesTable[256] = {
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
/* 0xF0 */2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, };

const u8 NesCpu::sizeTable[256] = {
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
/* 0xF0 */2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3 };

const u8 NesCpu::addressingModeTable[256] = {
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
/* 0xF8 */Impli, AbsoY, Impli, AbsoY, AbsoX, AbsoX, AbsoX, AbsoX };

const char *NesCpu::nameTable[256] = {
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
/* 0xF8 */"SED", "SBC", "NOP", "ISC", "TOP", "SBC", "INC", "ISC" };
