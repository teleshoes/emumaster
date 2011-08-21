#include "nescpu.h"
#include "nesmachine.h"
#include "nescpumapper.h"
#include "nesapu.h"
#include "nesmapper.h"
#include <QDebug>

NesCpu::NesCpu(NesMachine *machine) :
	M6502(machine),
	m_mapper(0),
	m_reset(true) {
	m_apu = new NesApu(this);
	QObject::connect(m_apu, SIGNAL(request_irq_o(bool)), SLOT(apu_irq_i(bool)));
}

NesCpu::~NesCpu() {
}

void NesCpu::setMapper(NesCpuMapper *mapper) {
	m_mapper = mapper;
	QObject::connect(mapper, SIGNAL(request_irq_o(bool)), SLOT(mapper_irq_i(bool)));
}

void NesCpu::write(quint16 address, quint8 data)
{ m_mapper->write(address, data); }
quint8 NesCpu::read(quint16 address)
{ return m_mapper->read(address); }

NesMachine *NesCpu::machine() const
{ return static_cast<NesMachine *>(parent()); }

uint NesCpu::clock(uint cycles) {
	uint executedCycles = 0;
	if (m_reset) {
		m_apuIrq = false;
		m_mapperIrq = false;
		irq0_i(false);

		while (executedCycles < cycles)
			executedCycles += executeOne();
		m_apu->reset();
		m_dmaCycles = 0;
	} else {
		if (m_dmaCycles) {
			if (cycles <= m_dmaCycles) {
				m_dmaCycles -= cycles;
				m_mapper->clock(cycles);
				return cycles;
			} else {
				executedCycles += m_dmaCycles;
				m_dmaCycles = 0;
				m_mapper->clock(executedCycles);
			}
		}
		while (executedCycles < cycles) {
			uint instrCycles = executeOne();
			// TODO mapper clock enable
			m_mapper->clock(instrCycles);
			executedCycles += instrCycles;
//			m_apu->clockFrameCounter(instrCycles);
		}
		m_apu->clockFrameCounter(executedCycles);
	}
	return executedCycles;
}

void NesCpu::dma(uint cycles)
{ m_dmaCycles += cycles; }

void NesCpu::nes_reset_i(bool on) {
	reset_i(on);
	m_reset = on;
}

void NesCpu::apu_irq_i(bool on) {
	bool oldIrqState = (m_apuIrq || m_mapperIrq);
	m_apuIrq = on;
	bool newIrqState = (m_apuIrq || m_mapperIrq);
	if (newIrqState != oldIrqState)
		irq0_i(newIrqState);
}

void NesCpu::mapper_irq_i(bool on) {
	bool oldIrqState = (m_apuIrq || m_mapperIrq);
	m_mapperIrq = on;
	bool newIrqState = (m_apuIrq || m_mapperIrq);
	if (newIrqState != oldIrqState)
		irq0_i(newIrqState);
}
