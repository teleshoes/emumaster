#ifndef NESCPUREC_FLAGS_H
#define NESCPUREC_FLAGS_H

#include "cpurec_p.h"

/*!
	Sets the given \a flag in the CPSR register of host processor to
	the given value \a on.
 */
inline void NesCpuTranslator::mSetInternalFlag(int flag, bool on)
{
	__ mrs(mWT, CPSR);
	if (on)
		__ orr(mWT, mWT, Operand(flag));
	else
		__ bic(mWT, mWT, Operand(flag));
	__ msr(CPSR_f, Operand(mWT));
}

/*!
	Sets the given \a flag in the register containing flags of 6502.P
	to the given value \a on.
 */
inline void NesCpuTranslator::mSetExternalFlag(int flag, bool on)
{
	Q_ASSERT(!(flag & (NesCpuBase::Negative|NesCpuBase::Overflow|
					   NesCpuBase::Zero|NesCpuBase::Carry)));
	if (on)
		__ orr(mFlags, mFlags, Operand(flag));
	else
		__ bic(mFlags, mFlags, Operand(flag));
}

inline void NesCpuTranslator::mClc() { mSetInternalFlag(kCConditionFlagBit, false); }
inline void NesCpuTranslator::mSec() { mSetInternalFlag(kCConditionFlagBit, true);  }

inline void NesCpuTranslator::mClv() { mSetInternalFlag(kVConditionFlagBit, false); }

inline void NesCpuTranslator::mCld() { mSetExternalFlag(NesCpuBase::Decimal, false); }
inline void NesCpuTranslator::mSed() { mSetExternalFlag(NesCpuBase::Decimal, true);  }

inline void NesCpuTranslator::mCli() { mSetExternalFlag(NesCpuBase::IrqDisable, false); }
inline void NesCpuTranslator::mSei() { mSetExternalFlag(NesCpuBase::IrqDisable, true);  }

/*!
	Packs internal and external flags to 6502 format.
 */
inline void NesCpuTranslator::mPackFlags(Register reg)
{
	__ orr(reg, mFlags, Operand(NesCpuBase::Negative), LeaveCC, mi);
	__ orr(reg, reg, Operand(NesCpuBase::Overflow), LeaveCC, vs);
	__ orr(reg, reg, Operand(NesCpuBase::Zero), LeaveCC, eq);
	__ orr(reg, reg, Operand(NesCpuBase::Carry), LeaveCC, cs);
}

/*!
	Unpacks flags from 6502 format to internal and external flags.
 */
inline void NesCpuTranslator::mUnpackFlags(Register reg, Register tmp)
{
	Q_ASSERT(tmp != ip && tmp != reg);
	// read host flags
	__ mrs(tmp, CPSR);

	// transfer reg -> internal flags
	int nmask = kNConditionFlagBit|kZConditionFlagBit|kCConditionFlagBit|kVConditionFlagBit;
	__ bic(tmp, tmp, Operand(nmask));
	__ mov(ip, Operand(reg, LSL, 24+1), SetCC);
	__ orr(tmp, tmp, Operand(kNConditionFlagBit), LeaveCC, cs);
	__ orr(tmp, tmp, Operand(kVConditionFlagBit), LeaveCC, mi);
#if defined(CAN_USE_ARMV7_INSTRUCTIONS)
	// carry and zero flag are placed side by side in both (ARM and 6502) register flags
	// so we can use bfi instruction to transfer them
	__ bfi(tmp, reg, 29, 2);
#else
	__ mov(ip, Operand(reg, LSL, 24+7), SetCC);
	__ orr(tmp, tmp, Operand(kZConditionFlagBit), LeaveCC, cs);
	__ orr(tmp, tmp, Operand(kCConditionFlagBit), LeaveCC, mi);
#endif

	// transfer reg -> external flags
	int mask = NesCpuBase::IrqDisable | NesCpuBase::Decimal | NesCpuBase::Break;
	__ and_(mFlags, reg, Operand(mask));
	// Unused flag is always set to one when pushing onto stack

	// apply new host flags
	__ msr(CPSR_f, Operand(tmp));
}

#endif // NESCPUREC_FLAGS_H
