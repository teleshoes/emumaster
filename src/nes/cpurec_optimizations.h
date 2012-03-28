#ifndef NESCPUREC_OPTIMIZATIONS_H
#define NESCPUREC_OPTIMIZATIONS_H

#include "cpurec_p.h"

bool NesCpuTranslator::mTryOptimize()
{
	u8 op = nesCpuReadDirect(m_recPc);
	bool optimized;
	switch (op) {
	case 0x4c:	optimized = mOptimJmpAbs(); break;
	case 0xad:	optimized = mOptimLdaAbs(); break;
	case 0x88:	optimized = mOptimizeDecReg(0x88, mY); break;
	case 0xca:	optimized = mOptimizeDecReg(0xca, mX); break;
	case 0xc8:	optimized = mOptimizeIncReg(0xc8, mY); break;
	case 0xe8:	optimized = mOptimizeIncReg(0xe8, mX); break;
	default:	optimized = false; break;
	}
	return optimized;
}

bool NesCpuTranslator::mOptimJmpAbs()
{
	u8 lo = nesCpuReadDirect(m_recPc+1);
	u8 hi = nesCpuReadDirect(m_recPc+2);
	u16 dst = lo | (hi << 8);

	if (dst != m_recPc)
		return false;

	Label begin;
	__ bind(&begin);
	mSaturateCycles(3);
	__ mov(r0, Operand(m_recPc));
	__ bl(&m_syncLabel);
	__ b(&begin);
	m_recPc += 3;
	return true;
}

bool NesCpuTranslator::mOptimLdaAbs()
{
	u8 lo = nesCpuReadDirect(m_recPc+1);
	u8 hi = nesCpuReadDirect(m_recPc+2);
	u16 addr = lo | (hi << 8);

	if (addr != 0x2002) // PPU Status Register
		return false;

	u8 nextOp = nesCpuReadDirect(m_recPc+3);
	if (nextOp == 0x10) // bpl
		return mOptimLdaAbsBpl();

	if (nextOp == 0x29) // and
		return mOptimLdaAbsAndImm();

	return false;
}

bool NesCpuTranslator::mOptimLdaAbsBpl()
{
	u8 disp = nesCpuReadDirect(m_recPc+4);
	if (disp != 0xfb)
		return false;

	Label begin;
	Label end;
	__ bind(&begin);
	mRead8(0x2002, mDT, 0);
	mLda();
	mAddCycles(3+2, mi);
	__ b(&end, mi);
	u16 ET = m_recPc + 5;
	u16 EA = m_recPc;
	int cycles = 3+2+1;
	bool boundary = ((ET^EA) >> 8) & 1;
	if (boundary)
		cycles++;
	mSaturateCycles(cycles);
	__ mov(r0, Operand(m_recPc));
	__ bl(&m_syncLabel);
	__ b(&begin);
	__ bind(&end);
	m_recPc += 5;
	return true;
}

bool NesCpuTranslator::mOptimLdaAbsAndImm()
{
	u8 nextOp = nesCpuReadDirect(m_recPc+5);
	if (nextOp == 0xd0 || nextOp == 0xf0) {
		Condition cond = ((nextOp == 0xd0) ? ne : eq);
		return mOptimLdaAbsAndImmBneBeq(cond);
	}
	return false;
}

bool NesCpuTranslator::mOptimLdaAbsAndImmBneBeq(Condition cond)
{
	u8 disp = nesCpuReadDirect(m_recPc+6);
	if (disp != 0xf9)
		return false;

	Label begin;
	Label end;
	__ bind(&begin);
	// lda, and
	mRead8(0x2002, mDT, 0);
	__ mov(mA, Operand(mDT, LSL, 24));
	__ mov(mDT, Operand(nesCpuReadDirect(m_recPc+4) << 24)); // do not touch P.C
	__ and_(mA, mA, Operand(mDT), SetCC);
	// bxx
	mAddCycles(3+2+2, NegateCondition(cond));
	__ b(&end, NegateCondition(cond));
	u16 ET = m_recPc + 7;
	u16 EA = m_recPc;
	int cycles = 3+2+2+1;
	bool boundary = ((ET^EA) >> 8) & 1;
	if (boundary)
		cycles++;
	mSaturateCycles(cycles);
	__ mov(r0, Operand(m_recPc));
	__ bl(&m_syncLabel);
	__ b(&begin);
	__ bind(&end);
	m_recPc += 7;
	return true;
}

bool NesCpuTranslator::mOptimizeIncReg(u8 op, Register reg)
{
	int count = 1;
	while (nesCpuReadDirect(m_recPc + count) == op)
		count++;

	mIncRegMultiple(reg, count);
	mAddCycles(count * 2);
	m_recPc += count;
	return true;
}

bool NesCpuTranslator::mOptimizeDecReg(u8 op, Register reg)
{
	int count = 1;
	while (nesCpuReadDirect(m_recPc + count) == op)
		count++;

	mDecRegMultiple(reg, count);
	mAddCycles(count * 2);
	m_recPc += count;
	return true;
}

void NesCpuTranslator::mSaturateCycles(int modValue)
{
	if (modValue == 8) { // uncomment if needed || modValue == 4 || modValue == 2) {
		// a clever trick to jump over the saturation if mCycles >= 0
		__ mov(ip, Operand(mCycles, LSR, 31));
		__ eor(ip, ip, Operand(1));
		__ add(pc, pc, Operand(ip, LSL, 2));
		// some pad needed - write current pc
		__ dd(currentPc());
		// if mCycles negative - saturate it
		__ and_(mCycles, mCycles, Operand(modValue-1));
		// continue otherwise
	} else if (modValue == 3) {
		__ mov(r3, Operand(1431655766));
		__ smull(r2, ip, r3, mCycles);
		__ sub(r1, ip, Operand(mCycles, ASR, 31));
		__ add(r3, r1, Operand(r1, LSL, 1));
		__ rsb(mCycles, r3, Operand(mCycles));
		__ add(mCycles, mCycles, Operand(3));
	} else {
		__ mrs(ip, CPSR);
		Label loop;
		__ bind(&loop);
		__ add(mCycles, mCycles, Operand(modValue), SetCC);
		__ b(&loop, mi);
		__ msr(CPSR_f, Operand(ip));
	}
}

#endif // NESCPUREC_OPTIMIZATIONS_H
