#ifndef NESCPU_H
#define NESCPU_H

class NesMachine;
class NesCpuMapper;
class NesApu;
#include "nes_global.h"
#include <cpu/m6502.h>

class NES_EXPORT NesCpu : public M6502 {
	Q_OBJECT
public:
	explicit NesCpu(NesMachine *machine);
	~NesCpu();
	void setMapper(NesCpuMapper *mapper);
	NesMachine *machine() const;
	NesApu *apu() const;
	uint clock(uint cycles);
	void dma(uint cycles);
	//TODO save
	// TODO load
public slots:
	void nes_reset_i(bool on);
	void apu_irq_i(bool on);
	void mapper_irq_i(bool on);
protected:
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);
private:
	NesCpuMapper *m_mapper;
	NesApu *m_apu;
	bool m_reset;
	uint m_dmaCycles;

	bool m_apuIrq;
	bool m_mapperIrq;

	friend class NesApu;
};

inline NesApu *NesCpu::apu() const
{ return m_apu; }

#endif // NESCPU_H
