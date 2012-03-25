#ifndef NESCPUREC_LOGICAL_H
#define NESCPUREC_LOGICAL_H

#include "cpurec_p.h"

/*!
	A = A & DT
	P.Z = (A == 0)
	P.N = A[7]
 */
inline void NesCpuTranslator::mAnd()
{
	// one more step is needed to shift the data and leave carry flag unchanged
	__ mov(mWT, Operand(mDT, LSL, 24)); // do not touch P.C
	__ and_(mA, mA, Operand(mWT), SetCC);
}

/*!
	A = A | DT
	P.Z = (A == 0)
	P.N = A[7]
 */
inline void NesCpuTranslator::mOra()
{
	__ mov(mWT, Operand(mDT, LSL, 24)); // do not touch P.C
	__ orr(mA, mA, Operand(mWT), SetCC);
}

/*!
	A = A ^ DT
	P.Z = (A == 0)
	P.N = A[7]
 */
inline void NesCpuTranslator::mEor()
{
	__ mov(mWT, Operand(mDT, LSL, 24)); // do not touch P.C
	__ eor(mA, mA, Operand(mWT), SetCC);
}

/*!
	P.Z = ((A&DT) == 0)
	P.N = DT[7]
	P.V = DT[6]
 */
inline void NesCpuTranslator::mBit()
{
	__ mrs(mWT, CPSR);
	uint nmask = kNConditionFlagBit|kZConditionFlagBit|kVConditionFlagBit;
	__ bic(mWT, mWT, Operand(nmask)); // save P.C clear P.Z P.N P.V
	__ tst(mA, Operand(mDT, LSL, 24));
	__ orr(mWT, mWT, Operand(kZConditionFlagBit), LeaveCC, eq); // update P.Z
	__ mov(mDT, Operand(mDT, LSL, 24+1), SetCC);
	__ orr(mWT, mWT, Operand(kNConditionFlagBit), LeaveCC, cs); // update P.N
	__ orr(mWT, mWT, Operand(kVConditionFlagBit), LeaveCC, mi); // update P.V
	__ msr(CPSR_f, Operand(mWT));
}

#endif // NESCPUREC_LOGICAL_H
