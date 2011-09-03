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
	static const int NmiVectorAddress		= 0xFFFA;
	static const int ResetVectorAddress		= 0xFFFC;
	static const int IrqVectorAddress		= 0xFFFE;

	explicit NesCpu(NesMachine *machine);
	~NesCpu();

	NesCpuMapper *mapper() const;
	void setMapper(NesCpuMapper *mapper);

	NesMachine *machine() const;
	NesApu *apu() const;
	uint clock(uint cycles);
	void dma(uint cycles);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
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

inline NesCpuMapper *NesCpu::mapper() const
{ return m_mapper; }

inline NesApu *NesCpu::apu() const
{ return m_apu; }

#endif // NESCPU_H
