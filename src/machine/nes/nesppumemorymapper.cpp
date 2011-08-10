#include "nesppumemorymapper.h"
#include "nesmapper.h"
#include "nesmachine.h"
#include "nesppu.h"
#include "nesdisk.h"

NesPpuMemoryMapper::NesPpuMemoryMapper(NesMapper *mapper) :
	QObject(mapper) {
	NesMachine *machine = mapper->machine();
	NesDisk *disk = machine->disk();
	m_rom = disk->vrom();
	Q_ASSERT(m_rom != 0);
	m_romSize = disk->vromSize();
	m_defaultMirroring = disk->mirroring();
	m_palette = machine->ppu()->palette();

	setRomBank(0);
	setMirroring(m_defaultMirroring);
}

void NesPpuMemoryMapper::write(quint16 address, quint8 data) {
	if ((address & 0x3F00) == 0x3F00)
		m_palette->write(address & 0x1F, data);
	else
		m_banks[(address >> 10) & 0xF][address & 0x3FF] = data;
}

quint8 NesPpuMemoryMapper::read(quint16 address) {
	if ((address & 0x3F00) == 0x3F00)
		return m_palette->read(address & 0x1F);
	else
		return m_banks[(address >> 10) & 0xF][address & 0x3FF];
}

void NesPpuMemoryMapper::setMirroring(Mirroring mirroring) {
	if (mirroring == Vertical)
		setMirroring(0, 1, 0, 1);
	else if (mirroring == Horizontal)
		setMirroring(0, 0, 1, 1);
	else if (mirroring == SingleHigh)
		setMirroring(1, 1, 1, 1);
	else if (mirroring == SingleLow)
		setMirroring(0, 0, 0, 0);
}

void NesPpuMemoryMapper::setMirroring(int bank0, int bank1, int bank2, int bank3) {
	m_banks[8] = m_banks[12] = m_ram + (bank0 ? 0x400 : 0);
	m_banks[9] = m_banks[13] = m_ram + (bank1 ? 0x400 : 0);
	m_banks[10] = m_banks[14] = m_ram + (bank2 ? 0x400 : 0);
	m_banks[11] = m_banks[15] = m_ram + (bank3 ? 0x400 : 0);
}
