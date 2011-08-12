#ifndef NESCPU_H
#define NESCPU_H

class NesMachine;
class NesCpuMemoryMapper;
class NesApu;
#include "nes_global.h"
#include <cpu/m6502.h>

class NES_EXPORT NesCpu : public M6502 {
	Q_OBJECT
public:
	explicit NesCpu(NesMachine *machine);
	void setMemory(NesCpuMemoryMapper *memory);
	NesMachine *machine() const;
	NesApu *apu() const;
	void clockTo(quint64 endCycle);
public slots:
	void reset_i(bool on);
protected:
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);
private:
	NesCpuMemoryMapper *m_memory;
	NesApu *m_apu;
	bool m_reset;

	friend class NesApu;
};

inline NesApu *NesCpu::apu() const
{ return m_apu; }

#endif // NESCPU_H
