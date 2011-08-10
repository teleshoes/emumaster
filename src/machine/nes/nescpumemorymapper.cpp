#include "nescpumemorymapper.h"
#include "nesmapper.h"
#include "nesmachine.h"
#include "nescpu.h"
#include "nesppu.h"
#include "nesapu.h"
#include "nespad.h"
#include "nesdisk.h"

NesCpuMemoryMapper::NesCpuMemoryMapper(NesMapper *mapper) :
	QObject(mapper) {
	NesMachine *machine = mapper->machine();
	NesDisk *disk = machine->disk();
	m_romData = disk->rom();
	m_romSize = disk->romSize();

	setRomBank(0);
	m_last4016Data = 0;

	m_cpu = machine->cpu();
	m_apu = m_cpu->apu();
	m_ppuRegisters = machine->ppu()->registers();
	m_pad0 = machine->pad0();
	m_pad1 = machine->pad1();
}

NesMapper *NesCpuMemoryMapper::mapper() const
{ return static_cast<NesMapper *>(parent()); }

void NesCpuMemoryMapper::write(quint16 address, quint8 data) {
	if (address < 0x2000) {
		m_ram[address & 0x07FF] = data;
	} else if (address < 0x4000) {
		return m_ppuRegisters->write(address & 7, data);
	} else if (address < 0x4018) {
		if (address == 0x4016) {
			if (!(data & 1) && (m_last4016Data & 1)) {
				m_pad0->reset();
				m_pad1->reset();
			}
			m_last4016Data = data;
		} else if (address == 0x4014) {
			spriteRamDma(data);
		} else {
			m_apu->write(address & 0x1F, data);
		}
	} else if (address & 0x8000) {
		writeHigh(address, data);
	}
	//	TODO 4018h-5FFFh   Cartridge Expansion Area almost 8K
	//	TODO 6000h-7FFFh   Cartridge SRAM Area 8K
}

quint8 NesCpuMemoryMapper::read(quint16 address) {
	if (address & 0x8000) {
		return m_romBanks[(address >> 13) & 3][address & 0x1FFF];
	} else if (address < 0x2000) {
		return m_ram[address & 0x07FF];
	} else if (address < 0x4000) {
		return m_ppuRegisters->read(address & 7);
	} else if (address < 0x4018) {
		if (address >= 0x4016) {
			quint8 ret;
			if (address == 0x4016)
				ret = m_pad0->read();
			else
				ret = m_pad1->read();
			return ret | 0x40;
		}
		return m_apu->read(address & 0x1F);
	}
	//	TODO 4018h-5FFFh   Cartridge Expansion Area almost 8K
	//	TODO 6000h-7FFFh   Cartridge SRAM Area 8K
	return 0;
}

void NesCpuMemoryMapper::spriteRamDma(int page) {
	uint address = page << 8;
	for (int i = 0; i < 256; i++)
		m_ppuRegisters->write(4, read(address + i));
	m_cpu->stoleCycles(513);
}

void NesCpuMemoryMapper::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	Q_UNUSED(data)
}
