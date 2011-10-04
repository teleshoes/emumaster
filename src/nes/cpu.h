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

#ifndef NESCPU_H
#define NESCPU_H

#include "mapper.h"

class NesCpu {
public:
	static const int NmiVectorAddress		= 0xFFFA;
	static const int ResetVectorAddress		= 0xFFFC;
	static const int IrqVectorAddress		= 0xFFFE;

	void init();

	u32 executeOne();

	u32 clock(u32 cycles);
	void dma(u32 cycles);

	void WRITE(u16 address, u8 data);
	u8 READ(u16 address);
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

	static const u8 cyclesTable[256];
	static const u8 sizeTable[256];
	static const u8 addressingModeTable[256];
	static const char *nameTable[256];
};

inline void NesCpu::WRITE(u16 address, u8 data)
{ nesMapper->write(address, data); }
inline u8 NesCpu::READ(u16 address)
{ return nesMapper->read(address); }

extern NesCpu nesCpu;

#endif // NESCPU_H
