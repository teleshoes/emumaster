#include "nescpumapper.h"
#include "nesmapper.h"
#include "nesmachine.h"
#include "nesdisk.h"
#include "nescpu.h"
#include "nesppu.h"
#include "nesapu.h"
#include "nespad.h"

NesCpuMapper::NesCpuMapper(NesMapper *mapper) :
	QObject(mapper) {
	NesMachine *machine = mapper->machine();
	m_disk = machine->disk();
	m_rom = m_disk->rom();
	m_romSize = m_disk->romSize();

	m_cpu = machine->cpu();
	m_ppu = machine->ppu();
	m_ppuRegisters = m_ppu->registers();
	m_apu = m_cpu->apu();
	m_pad = machine->pad();

	qMemSet(m_ram, 0, sizeof(m_ram));
	if (m_disk->crc() == 0x29401686) // Minna no Taabou no Nakayoshi Dai Sakusen(J)
		qMemSet(m_ram, 0xFF, sizeof(m_ram));

	if (!m_disk->hasBatteryBackedRam() && m_disk->mapperType() != 20)
		qMemSet(m_wram, 0xFF, sizeof(m_wram));

	if (m_disk->hasTrainer())
		qMemCopy(m_wram + 0x1000, m_disk->trainer(), 512);

	qMemSet(m_banks, 0, sizeof(m_banks));
	m_banks[0] = m_ram;
	m_banks[1] = m_xram;
	m_banks[2] = m_xram;
	m_banks[3] = m_wram;

	m_irqOut = false;

	setRom32KBank(0);
}

NesCpuMapper::~NesCpuMapper() {
}

NesMapper *NesCpuMapper::mapper() const
{ return static_cast<NesMapper *>(parent()); }

void NesCpuMapper::write(quint16 address, quint8 data) {
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
		else
			writeLow(address, data);
		break;
	case 3: // 0x6000-0x7FFF
		writeLow(address, data);
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

quint8 NesCpuMapper::read(quint16 address) {
	quint8 data;
	switch (address >> 13) {
	case 0:	// 0x0000-0x1FFF
		data = m_ram[address & 0x07FF];
		break;
	case 1:	// 0x2000-0x3FFF
		data = m_ppuRegisters->read(address & 7);
		break;
	case 2:	// 0x4000-0x5FFF
		if (address < 0x4100)
			data = readReg(address);
		else
			data = readLow(address);
		break;
	case 3:	// 0x6000-0x7FFF
		data = readLow(address);
		break;
	case 4:	// 0x8000-0x9FFF
	case 5:	// 0xA000-0xBFFF
	case 6:	// 0xC000-0xDFFF
	case 7:	// 0xE000-0xFFFF
		data = m_banks[address >> 13][address & 0x1FFF];
		break;
	}
	return data;
}

void NesCpuMapper::writeLow(quint16 address, quint8 data) {
	if (address >= 0x6000) // < 0x8000
		m_banks[address >> 13][address & 0x1FFF] = data;
}

quint8 NesCpuMapper::readLow(quint16 address) {
	if (address >= 0x6000) // < 0x8000
		return m_banks[address >> 13][address & 0x1FFF];
	return address >> 8;
}

void NesCpuMapper::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	Q_UNUSED(data)
}

void NesCpuMapper::writeReg(quint16 address, quint8 data) {
	if (address == 0x4014) {
		m_ppu->dma(this, data);
		m_cpu->dma(514);
	} else if (address == 0x4016) {
		m_pad->write(0, data);
	} else if (address == 0x4017) {
		m_apu->write(0x17, data);
		m_pad->write(1, data);
	} else if (address < 0x4017) {
		m_apu->write(address & 0x1F, data);
	} else {
		writeEx(address, data);
	}
}

quint8 NesCpuMapper::readReg(quint16 address) {
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
		return m_apu->read(address & 0x1F);
	} else {
		return readEx(address);
	}
}

void NesCpuMapper::writeEx(quint16 address, quint8 data) {
	Q_UNUSED(address)
	Q_UNUSED(data)
}

quint8 NesCpuMapper::readEx(quint16 address) {
	Q_UNUSED(address)
	return 0x00;
}

void NesCpuMapper::clock(uint cycles)
{ Q_UNUSED(cycles) }

void NesCpuMapper::setIrqSignalOut(bool on) {
	if (on != m_irqOut) {
		m_irqOut = on;
		emit request_irq_o(on);
	}
}

bool NesCpuMapper::save(QDataStream &s) {
	for (int i = 0; i < 8; i++) {
		quint8 *bank = m_banks[i];
		quint8 type;
		quint16 offset;
		if (bank >= m_rom && bank < (m_rom + m_romSize)) {
			type = 0;
			offset = bank - m_rom;
		} else if (bank >= m_ram && bank < (m_ram + sizeof(m_ram))) {
			type = 1;
			offset = bank - m_ram;
		} else if (bank >= m_wram && bank < (m_wram + sizeof(m_wram))) {
			type = 2;
			offset = bank - m_wram;
		} else if (bank >= m_xram && bank < (m_xram + sizeof(m_xram))) {
			type = 3;
			offset = bank - m_xram;
		} else {
			return false;
		}
		s << type;
		s << offset;
	}
	if (s.writeRawData(reinterpret_cast<const char *>(m_ram), sizeof(m_ram)) != sizeof(m_ram))
		return false;
	if (s.writeRawData(reinterpret_cast<const char *>(m_wram), sizeof(m_wram)) != sizeof(m_wram))
		return false;
	if (s.writeRawData(reinterpret_cast<const char *>(m_xram), sizeof(m_xram)) != sizeof(m_xram))
		return false;
	s << m_irqOut;
	return true;
}

bool NesCpuMapper::load(QDataStream &s) {
	for (int i = 0; i < 8; i++) {
		quint8 type;
		quint16 offset;
		s >> type;
		s >> offset;
		if (type == 0)
			m_banks[i] = m_rom + offset;
		else if (type == 1)
			m_banks[i] = m_ram + offset;
		else if (type == 2)
			m_banks[i] = m_wram + offset;
		else if (type == 3)
			m_banks[i] = m_xram + offset;
		else
			return false;
	}
	if (s.readRawData(reinterpret_cast<char *>(m_ram), sizeof(m_ram)) != sizeof(m_ram))
		return false;
	if (s.readRawData(reinterpret_cast<char *>(m_wram), sizeof(m_wram)) != sizeof(m_wram))
		return false;
	if (s.readRawData(reinterpret_cast<char *>(m_xram), sizeof(m_xram)) != sizeof(m_xram))
		return false;
	s >> m_irqOut;
	return true;
}
