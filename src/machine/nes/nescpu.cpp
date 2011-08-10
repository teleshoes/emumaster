#include "nescpu.h"
#include "nesmachine.h"
#include "nescpumemorymapper.h"
#include "nesapu.h"
#include <QDebug>

NesCpu::NesCpu(NesMachine *machine) :
	M6502(machine),
	m_memory(0),
	m_reset(true) {
	m_apu = new NesApu(this);
	QObject::connect(m_apu, SIGNAL(request_irq_o(bool)), SLOT(irq0_i(bool)));
}

void NesCpu::setMemory(NesCpuMemoryMapper *memory)
{ m_memory = memory; }

void NesCpu::write(quint16 address, quint8 data)
{ m_memory->write(address, data); }
quint8 NesCpu::read(quint16 address)
{ return m_memory->read(address); }

NesMachine *NesCpu::machine() const
{ return static_cast<NesMachine *>(parent()); }

void NesCpu::reset_i(bool on) {
	M6502::reset_i(on);
	m_reset = on;
}

void NesCpu::runTo(quint64 endCycle) {
	if (m_reset) {
		while (cycle() < endCycle)
			executeOne();
		m_apu->reset();
	} else {
		while (cycle() < endCycle) {
			quint64 prv = cycle();
			executeOne();
			quint64 now = cycle();
			m_apu->clockFrameCounter(now - prv);
		}
	}
}
