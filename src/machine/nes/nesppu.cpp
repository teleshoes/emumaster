#include "nesppu.h"
#include "nesmachine.h"
#include "nesppumemorymapper.h"

NesPpu::NesPpu(NesMachine *machine) :
	Nes2C0XPpu(machine),
	m_memory(0) {
}

void NesPpu::setMemory(NesPpuMemoryMapper *memory)
{ m_memory = memory; }

void NesPpu::write(quint16 address, quint8 data)
{ m_memory->write(address, data); }
quint8 NesPpu::read(quint16 address)
{ return m_memory->read(address); }
