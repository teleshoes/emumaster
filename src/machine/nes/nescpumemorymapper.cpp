#include "nescpumemorymapper.h"
#include "nesmapper.h"
#include "nesmachine.h"
#include "nesdisk.h"
#include "nescpu.h"
#include "nesppu.h"
#include "nesapu.h"
#include "nespad.h"

NesCpuMemoryMapper::NesCpuMemoryMapper(NesMapper *mapper) :
	QObject(mapper) {
	qMemSet(m_romBanks, 0, sizeof(m_romBanks));
	NesMachine *machine = mapper->machine();
	m_disk = machine->disk();
	m_romData = m_disk->rom();
	m_romSize = m_disk->romSize();

	m_cpu = machine->cpu();
	m_apu = m_cpu->apu();
	m_ppu = machine->ppu();
	m_ppuRegisters = m_ppu->registers();
	m_pad = machine->pad0();
}

void NesCpuMemoryMapper::reset() {
	setRomBank(0);
	m_last4016Data = 0;
//	NesDisk *disk = mapper()->machine()->disk();
//	TODO trainer if (disk->hasTrainer())
//		qMemCopy(m_catridgeRam, disk->)
}

NesMapper *NesCpuMemoryMapper::mapper() const
{ return static_cast<NesMapper *>(parent()); }

// TODO try to make inline
void NesCpuMemoryMapper::write(quint16 address, quint8 data) {
	switch (address >> 13) {
	case 0: // 0x0000-0x1FFF
		m_ram[address & 0x07FF] = data;
		break;
	case 1: // 0x2000-0x3FFF
		// TODO nsf
		m_ppuRegisters->write(address & 7, data);
		break;
	case 2: // 0x4000-0x5FFF
		if (address < 0x4100)
			writeReg(address, data);
		break;
	case 3: // 0x6000-0x7FFF
		m_wram[address & 0x1FFF] = data;
		break;
	case 4: // 0x8000-0x9FFF
	case 5:	// 0xA000-0xBFFF
	case 6:	// 0xC000-0xDFFF
	case 7:	// 0xE000-0xFFFF
		writeHigh(address, data);
		// TODO geniecode
		break;
	}
}

void NesCpuMemoryMapper::writeReg(quint16 address, quint8 data) {
	if (address == 0x4014) {
		m_ppu->dma(data);
		m_cpu->dma(514);
	} else if (address == 0x4016) {
		m_pad->write(0, data);
	} else if (address == 0x4017) {
		m_apu->write(0x17, data);
		m_pad->write(1, data);
	} else if (address < 0x4017) {
		m_apu->write(address & 0x1F, data);
	}
}

// TODO try to make inline
quint8 NesCpuMemoryMapper::read(quint16 address) {
	if (address & 0x8000) {
		return m_romBanks[(address >> 13) & 3][address & 0x1FFF];
	} else if (address < 0x2000) {
		return m_ram[address & 0x07FF];
	} else if (address < 0x4000) {
		return m_ppuRegisters->read(address & 7);
	} else if (address < 0x4100) {
		if (address == 0x4014) {
			return 0x14;
		} if (address == 0x4016) {
			quint8 data = m_pad->read(0);
			if (m_disk->isVSSystem())
				return data;
			else
				return data | 0x40; //! TODO | m_TapeOut
		} else if (address == 0x4017) {
			quint8 data = m_pad->read(1);
			if (m_disk->isVSSystem())
				return data;
			else
				return data | m_apu->read(0x17);
		} else if (address < 0x4017) {
			return m_apu->read(address);
		}
		return m_apu->read(address & 0x1F);
	} else if (address < 0x6000) {
		//	TODO 4018h-5FFFh   Cartridge Expansion Area almost 8K
	} else {
		// TODO wram
	}
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

void NesCpuMemoryMapper::save(QDataStream &s) {
	Q_UNUSED(s)
	// TODO implement
}

bool NesCpuMemoryMapper::load(QDataStream &s) {
	Q_UNUSED(s)
	reset();
	// TODO implement
	return true;
}
