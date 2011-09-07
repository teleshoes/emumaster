#include "nesmapper.h"
#include "nesmachine.h"
#include "nesdisk.h"
#include "nescpu.h"
#include "nesppu.h"
#include "nesapu.h"
#include "nespad.h"
#include "gamegeniecode.h"
#include <QDataStream>

#include "mapper/mapper000.h"
#include "mapper/mapper001.h"
#include "mapper/mapper002.h"
#include "mapper/mapper003.h"
#include "mapper/mapper004.h"
#include "mapper/mapper005.h"
#include "mapper/mapper006.h"
#include "mapper/mapper007.h"
#include "mapper/mapper008.h"
#include "mapper/mapper009.h"
#include "mapper/mapper010.h"
#include "mapper/mapper011.h"
#include "mapper/mapper012.h"
#include "mapper/mapper013.h"
#include "mapper/mapper015.h"
#include "mapper/mapper016.h"
#include "mapper/mapper017.h"
#include "mapper/mapper018.h"
#include "mapper/mapper019.h"
#include "mapper/mapper021.h"
#include "mapper/mapper022.h"
#include "mapper/mapper023.h"
#include "mapper/mapper024.h"
#include "mapper/mapper025.h"
#include "mapper/mapper026.h"
#include "mapper/mapper027.h"
#include "mapper/mapper032.h"
#include "mapper/mapper033.h"
#include "mapper/mapper034.h"
#include "mapper/mapper040.h"
#include "mapper/mapper041.h"
#include "mapper/mapper042.h"
#include "mapper/mapper043.h"
#include "mapper/mapper044.h"
#include "mapper/mapper045.h"
#include "mapper/mapper046.h"
#include "mapper/mapper047.h"
#include "mapper/mapper048.h"
#include "mapper/mapper050.h"
#include "mapper/mapper051.h"
#include "mapper/mapper057.h"
#include "mapper/mapper058.h"
#include "mapper/mapper060.h"
#include "mapper/mapper061.h"
#include "mapper/mapper062.h"
#include "mapper/mapper064.h"
#include "mapper/mapper065.h"
#include "mapper/mapper066.h"
#include "mapper/mapper067.h"
#include "mapper/mapper068.h"
#include "mapper/mapper069.h"
#include "mapper/mapper070.h"
#include "mapper/mapper071.h"
#include "mapper/mapper200.h"
#include "mapper/mapper201.h"
#include "mapper/mapper202.h"
#include "mapper/mapper222.h"
#include "mapper/mapper225.h"
#include "mapper/mapper226.h"
#include "mapper/mapper227.h"
#include "mapper/mapper228.h"
#include "mapper/mapper229.h"
#include "mapper/mapper230.h"
#include "mapper/mapper231.h"
#include "mapper/mapper232.h"
#include "mapper/mapper233.h"
#include "mapper/mapper235.h"
#include "mapper/mapper236.h"
#include "mapper/mapper240.h"
#include "mapper/mapper241.h"
#include "mapper/mapper242.h"
#include "mapper/mapper243.h"
#include "mapper/mapper244.h"
#include "mapper/mapper246.h"
#include "mapper/mapper251.h"
#include "mapper/mapper252.h"
#include "mapper/mapper254.h"
#include "mapper/mapper255.h"

NesMapper::NesMapper() {
}

#define NES_MAPPER_CREATE_CASE(i,name) \
		mapper = new Mapper##i(); \
		mapper->m_name = name; \
		break

NesMapper *NesMapper::create(NesMachine *machine, quint8 type) {
	NesMapper *mapper = 0;
	switch (type) {
	case   0: NES_MAPPER_CREATE_CASE(000, "-");
	case   1: NES_MAPPER_CREATE_CASE(001, "MMC1");
	case   2: NES_MAPPER_CREATE_CASE(002, "UNROM");
	case   3: NES_MAPPER_CREATE_CASE(003, "CNROM");
	case   4: NES_MAPPER_CREATE_CASE(004, "MMC3");
	case   5: NES_MAPPER_CREATE_CASE(005, "MMC5");
	case   6: NES_MAPPER_CREATE_CASE(006, "FFE F4XX");
	case   7: NES_MAPPER_CREATE_CASE(007, "AOROM/AMROM");
	case   8: NES_MAPPER_CREATE_CASE(008, "FFE F3XXX");
	case   9: NES_MAPPER_CREATE_CASE(009, "MMC2");
	case  10: NES_MAPPER_CREATE_CASE(010, "MMC4");
	case  11: NES_MAPPER_CREATE_CASE(011, "Color Dreams");
	case  12: NES_MAPPER_CREATE_CASE(012, "DBZ5");
	case  13: NES_MAPPER_CREATE_CASE(013, "CPROM");
	case  15: NES_MAPPER_CREATE_CASE(015, "100-in-1");
	case  16: NES_MAPPER_CREATE_CASE(016, "Bandai Standard");
	case  17: NES_MAPPER_CREATE_CASE(017, "FFE F8XX");
	case  18: NES_MAPPER_CREATE_CASE(018, "Jaleco SS8806");
	case  19: NES_MAPPER_CREATE_CASE(019, "Namcot 106");
	case  21: NES_MAPPER_CREATE_CASE(021, "Konami VRC4");
	case  22: NES_MAPPER_CREATE_CASE(022, "Konami VRC2 type A");
	case  23: NES_MAPPER_CREATE_CASE(023, "Konami VRC2 type B");
	case  24: NES_MAPPER_CREATE_CASE(024, "Konami VRC6(Normal)");
	case  25: NES_MAPPER_CREATE_CASE(025, "Konami VRC4(Normal)");
	case  26: NES_MAPPER_CREATE_CASE(026, "Konami VRC6(PA0,PA1 reverse)");
	case  27: NES_MAPPER_CREATE_CASE(027, "Konami VRC4(World Hero)");
	case  32: NES_MAPPER_CREATE_CASE(032, "Irem G101");
	case  33: NES_MAPPER_CREATE_CASE(033, "Taito TC0190");
	case  34: NES_MAPPER_CREATE_CASE(034, "Nina-1");
	case  40: NES_MAPPER_CREATE_CASE(040, "SMB2J");
	case  41: NES_MAPPER_CREATE_CASE(041, "Caltron 6-in-1");
	case  42: NES_MAPPER_CREATE_CASE(042, "Mario Baby");
	case  43: NES_MAPPER_CREATE_CASE(043, "SMB2J");
	case  44: NES_MAPPER_CREATE_CASE(044, "Super HiK 7-in-1");
	case  45: NES_MAPPER_CREATE_CASE(045, "1000000-in-1");
	case  46: NES_MAPPER_CREATE_CASE(046, "Rumble Station");
	case  47: NES_MAPPER_CREATE_CASE(047, "NES-QJ");
	case  48: NES_MAPPER_CREATE_CASE(048, "Taito TC190V");
	case  50: NES_MAPPER_CREATE_CASE(050, "SMB2J");
	case  51: NES_MAPPER_CREATE_CASE(051, "11-in-1");
	case  57: NES_MAPPER_CREATE_CASE(057, "-");
	case  58: NES_MAPPER_CREATE_CASE(058, "-");
	case  60: NES_MAPPER_CREATE_CASE(060, "-");
	case  61: NES_MAPPER_CREATE_CASE(061, "-");
	case  62: NES_MAPPER_CREATE_CASE(062, "-");
	case  64: NES_MAPPER_CREATE_CASE(064, "Tengen Rambo-1");
	case  65: NES_MAPPER_CREATE_CASE(065, "Irem H3001");
	case  66: NES_MAPPER_CREATE_CASE(066, "Bandai 74161");
	case  67: NES_MAPPER_CREATE_CASE(067, "SunSoft Mapper 3");
	case  68: NES_MAPPER_CREATE_CASE(068, "SunSoft Mapper 4");
	case  69: NES_MAPPER_CREATE_CASE(069, "SunSoft FME-7");
	case  70: NES_MAPPER_CREATE_CASE(070, "Bandai 74161");
	case  71: NES_MAPPER_CREATE_CASE(071, "Camerica");
	case 200: NES_MAPPER_CREATE_CASE(200, "1200-in-1");
	case 201: NES_MAPPER_CREATE_CASE(201, "21-in-1");
	case 202: NES_MAPPER_CREATE_CASE(202, "150-in-1");
	case 222: NES_MAPPER_CREATE_CASE(222, "-");
	case 225: NES_MAPPER_CREATE_CASE(225, "72-in-1");
	case 226: NES_MAPPER_CREATE_CASE(226, "76-in-1");
	case 227: NES_MAPPER_CREATE_CASE(227, "1200-in-1");
	case 228: NES_MAPPER_CREATE_CASE(228, "Action 52");
	case 229: NES_MAPPER_CREATE_CASE(229, "31-in-1");
	case 230: NES_MAPPER_CREATE_CASE(230, "22-in-1");
	case 231: NES_MAPPER_CREATE_CASE(231, "20-in-1");
	case 232: NES_MAPPER_CREATE_CASE(232, "Quattro Games");
	case 233: NES_MAPPER_CREATE_CASE(233, "42-in-1");
	case 235: NES_MAPPER_CREATE_CASE(235, "150-in-1");
	case 236: NES_MAPPER_CREATE_CASE(236, "800-in-1");
	case 240: NES_MAPPER_CREATE_CASE(240, "Gen Ke Le Zhuan");
	case 241: NES_MAPPER_CREATE_CASE(241, "Fon Serm Bon");
	case 242: NES_MAPPER_CREATE_CASE(242, "Wai Xing Zhan Shi");
	case 243: NES_MAPPER_CREATE_CASE(243, "PC-Sachen/Hacker");
	case 244: NES_MAPPER_CREATE_CASE(244, "-");
	case 246: NES_MAPPER_CREATE_CASE(246, "Phone Serm Berm");
	case 251: NES_MAPPER_CREATE_CASE(251, "-");
	case 252: NES_MAPPER_CREATE_CASE(252, "-");
	case 254: NES_MAPPER_CREATE_CASE(254, "Pokemon Pirate Cart");
	case 255: NES_MAPPER_CREATE_CASE(255, "110-in-1");
	default: break;
	}
	if (mapper)
		mapper->setParent(machine);
	return mapper;
}

void NesMapper::reset() {
	m_disk = machine()->disk();
	m_rom = m_disk->rom();
	m_romSize = m_disk->romSize();

	m_cpu = machine()->cpu();
	m_ppu = machine()->ppu();
	m_ppuRegisters = m_ppu->registers();
	m_apu = m_cpu->apu();
	m_pad = machine()->pad();

	qMemSet(m_ram, 0, sizeof(m_ram));
	if (m_disk->crc() == 0x29401686) // Minna no Taabou no Nakayoshi Dai Sakusen(J)
		qMemSet(m_ram, 0xFF, sizeof(m_ram));

	if (!m_disk->hasBatteryBackedRam() && m_disk->mapperType() != 20)
		qMemSet(m_wram, 0xFF, sizeof(m_wram));

	if (m_disk->hasTrainer())
		qMemCopy(m_wram + 0x1000, m_disk->trainer(), 512);

	qMemSet(m_cpuBanks, 0, sizeof(m_cpuBanks));
	m_cpuBanks[0] = m_ram;
	m_cpuBanks[1] = m_xram;
	m_cpuBanks[2] = m_xram;
	m_cpuBanks[3] = m_wram;

	m_irqOut = false;

	setRom32KBank(0);

	processGameGenieCodes();

	qMemSet(m_ppuBanks, 0, sizeof(m_ppuBanks));
	for (int i = 0; i < 16; i++)
		m_ppuBanksType[i] = VramBank;
	qMemSet(m_vram, 0, sizeof(m_vram));
	qMemSet(m_cram, 0, sizeof(m_cram));

	m_vrom = disk()->vrom();
	Q_ASSERT(m_vrom != 0);
	m_vromSize = disk()->vromSize();
	m_defaultMirroring = disk()->mirroring();

	ppu()->setRenderMethod(NesPpu::PreRender);
	ppu()->setCharacterLatchEnabled(false);
	ppu()->setExternalLatchEnabled(false);

	setMirroring(m_defaultMirroring);
	setVrom8KBank(0);
}

void NesMapper::write(quint16 address, quint8 data) {
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
		processGameGenieCodes();
		break;
	}
}

quint8 NesMapper::read(quint16 address) {
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
		data = m_cpuBanks[address >> 13][address & 0x1FFF];
		break;
	}
	return data;
}

void NesMapper::writeLow(quint16 address, quint8 data) {
	if (address >= 0x6000) // < 0x8000
		m_cpuBanks[address >> 13][address & 0x1FFF] = data;
}

quint8 NesMapper::readLow(quint16 address) {
	if (address >= 0x6000) // < 0x8000
		return m_cpuBanks[address >> 13][address & 0x1FFF];
	return address >> 8;
}

void NesMapper::writeHigh(quint16 address, quint8 data) {
	Q_UNUSED(address)
	Q_UNUSED(data)
}

void NesMapper::writeReg(quint16 address, quint8 data) {
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
	} else {
		writeEx(address, data);
	}
}

quint8 NesMapper::readReg(quint16 address) {
	if (address == 0x4014) {
		return 0x14;
	} if (address == 0x4016) {
		quint8 data = m_pad->read(0);
		return data | 0x40; //! TODO | m_TapeOut
	} else if (address == 0x4017) {
		quint8 data = m_pad->read(1);
		return data | m_apu->read(0x17);
	} else if (address < 0x4017) {
		return m_apu->read(address & 0x1F);
	} else {
		return readEx(address);
	}
}

void NesMapper::writeEx(quint16 address, quint8 data) {
	Q_UNUSED(address)
	Q_UNUSED(data)
}

quint8 NesMapper::readEx(quint16 address) {
	Q_UNUSED(address)
	return 0x00;
}

void NesMapper::clock(uint cycles)
{ Q_UNUSED(cycles) }

void NesMapper::setIrqSignalOut(bool on) {
	if (on != m_irqOut) {
		m_irqOut = on;
		emit request_irq_o(on);
	}
}

void NesMapper::setGameGenieCodeList(const QList<GameGenieCode> &codes) {
	for (int i = 0; i < m_gameGenieCodeList.size(); i++) {
		const GameGenieCode &code = m_gameGenieCodeList.at(i);
		uint address = code.address() | 0x8000;
		if (readDirect(address) == code.replaceData())
			writeDirect(address, code.expectedData());
	}
	m_gameGenieCodeList = codes;
	processGameGenieCodes();
}

void NesMapper::processGameGenieCodes() {
	for (int i = 0; i < m_gameGenieCodeList.size(); i++) {
		GameGenieCode &code = m_gameGenieCodeList[i];
		uint address = code.address() | 0x8000;
		if (code.isEightCharWide()) {
			if (readDirect(address) == code.expectedData())
				writeDirect(address, code.replaceData());
		} else {
			code.setExpectedData(readDirect(address));
			writeDirect(address, code.replaceData());
		}
	}
}

void NesMapper::setMirroring(Mirroring mirroring) {
	if (mirroring == Vertical)
		setMirroring(0, 1, 0, 1);
	else if (mirroring == Horizontal)
		setMirroring(0, 0, 1, 1);
	else if (mirroring == SingleHigh)
		setMirroring(1, 1, 1, 1);
	else if (mirroring == SingleLow)
		setMirroring(0, 0, 0, 0);
	else if (mirroring == FourScreen)
		setMirroring(0, 1, 2, 3);
}

void NesMapper::setMirroring(uint bank0, uint bank1, uint bank2, uint bank3) {
	m_ppuBanks[8] = m_ppuBanks[12] = m_vram + bank0*0x400;
	m_ppuBanks[9] = m_ppuBanks[13] = m_vram + bank1*0x400;
	m_ppuBanks[10] = m_ppuBanks[14] = m_vram + bank2*0x400;
	m_ppuBanks[11] = m_ppuBanks[15] = m_vram + bank3*0x400;
}

void NesMapper::horizontalSync(int scanline)
{ Q_UNUSED(scanline) }

void NesMapper::verticalSync()
{ }

void NesMapper::addressBusLatch(quint16 address)
{ Q_UNUSED(address) }

void NesMapper::characterLatch(quint16 address)
{ Q_UNUSED(address) }

void NesMapper::extensionLatchX(uint x)
{ Q_UNUSED(x) }

void NesMapper::extensionLatch(quint16 address, quint8 *plane1, quint8 *plane2, quint8 *attribute) {
	Q_UNUSED(address)
	Q_UNUSED(plane1)
	Q_UNUSED(plane2)
	Q_UNUSED(attribute)
}

bool NesMapper::save(QDataStream &s) {
	// CPU
	for (int i = 0; i < 8; i++) {
		quint8 *bank = m_cpuBanks[i];
		quint8 type;
		uint offset;
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
	// PPU
	if (s.writeRawData(reinterpret_cast<const char *>(m_vram), sizeof(m_vram)) != sizeof(m_vram))
		return false;
	if (s.writeRawData(reinterpret_cast<const char *>(m_cram), sizeof(m_cram)) != sizeof(m_cram))
		return false;
	for (int i = 0; i < 16; i++) {
		s << quint8(m_ppuBanksType[i]);
		if (m_ppuBanksType[i] == VromBank)
			s << uint(m_ppuBanks[i] - m_vrom);
		else if (m_ppuBanksType[i] == CramBank)
			s << uint(m_ppuBanks[i] - m_cram);
		else if (m_ppuBanksType[i] == VramBank)
			s << uint(m_ppuBanks[i] - m_vram);
	}
	return true;
}

bool NesMapper::load(QDataStream &s) {
	// CPU
	for (int i = 0; i < 8; i++) {
		quint8 type;
		uint offset;
		s >> type;
		s >> offset;
		if (type == 0)
			m_cpuBanks[i] = m_rom + offset;
		else if (type == 1)
			m_cpuBanks[i] = m_ram + offset;
		else if (type == 2)
			m_cpuBanks[i] = m_wram + offset;
		else if (type == 3)
			m_cpuBanks[i] = m_xram + offset;
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
	// PPU
	if (s.readRawData(reinterpret_cast<char *>(m_vram), sizeof(m_vram)) != sizeof(m_vram))
		return false;
	if (s.readRawData(reinterpret_cast<char *>(m_cram), sizeof(m_cram)) != sizeof(m_cram))
		return false;
	for (int i = 0; i < 16; i++) {
		quint8 bType;
		s >> bType;
		m_ppuBanksType[i] = static_cast<BankType>(bType);

		uint offset;
		s >> offset;
		if (m_ppuBanksType[i] == VromBank)
			m_ppuBanks[i] = m_vrom + offset;
		else if (m_ppuBanksType[i] == CramBank)
			m_ppuBanks[i] = m_cram + offset;
		else if (m_ppuBanksType[i] == VramBank)
			m_ppuBanks[i] = m_vram + offset;
		else
			return false;
	}
	return true;
}
