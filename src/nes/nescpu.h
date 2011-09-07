#ifndef NESCPU_H
#define NESCPU_H

class NesMachine;
class NesMapper;
class NesApu;
#include "gamegeniecode.h"
#include "m6502.h"

class NesCpu : public M6502 {
	Q_OBJECT
public:
	explicit NesCpu(NesMachine *machine);
	~NesCpu();

	NesMapper *mapper() const;
	void setMapper(NesMapper *mapper);

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
	NesMapper *m_mapper;
	NesApu *m_apu;
	bool m_reset;
	uint m_dmaCycles;

	bool m_apuIrq;
	bool m_mapperIrq;

	friend class NesApu;
};

inline NesMapper *NesCpu::mapper() const
{ return m_mapper; }

inline NesApu *NesCpu::apu() const
{ return m_apu; }

#endif // NESCPU_H
