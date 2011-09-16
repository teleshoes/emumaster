#include "mapper.h"
#include "machine.h"
#include "disk.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "pad.h"
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

static  QList<GameGenieCode> nesGameGenieList;

#define NES_MAPPER_CREATE_CASE(i,name) \
		mapper = new Mapper##i(); \
		mapper->m_name = name; \
		break

NesMapper *NesMapper::create(NesMachine *machine, u8 type) {
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
	qMemSet(nesRam, 0, sizeof(nesRam));
	if (nesDiskCrc == 0x29401686) // Minna no Taabou no Nakayoshi Dai Sakusen(J)
		qMemSet(nesRam, 0xFF, sizeof(nesRam));

	if (!nesDisk.hasBatteryBackedRam() && nesMapperType != 20)
		qMemSet(nesWram, 0xFF, sizeof(nesWram));

	qMemCopy(nesWram + 0x1000, nesTrainer, 512);

	qMemSet(nesCpuBanks, 0, sizeof(nesCpuBanks));
	nesCpuBanks[0] = nesRam;
	nesCpuBanks[1] = nesXram;
	nesCpuBanks[2] = nesXram;
	nesCpuBanks[3] = nesWram;

	m_irqOut = false;

	setRom32KBank(0);

	processGameGenieCodes();

	qMemSet(nesPpuBanks, 0, sizeof(nesPpuBanks));
	for (int i = 0; i < 16; i++)
		nesPpuBanksType[i] = VramBank;
	qMemSet(nesVram, 0, sizeof(nesVram));
	qMemSet(nesCram, 0, sizeof(nesCram));

	ppu()->setRenderMethod(NesPpu::PreRender);
	ppu()->setCharacterLatchEnabled(false);
	ppu()->setExternalLatchEnabled(false);

	setMirroring(nesDefaultMirroring);
	setVrom8KBank(0);
}

void NesMapper::write(u16 address, u8 data) {
	switch (address >> 13) {
	case 0: // 0x0000-0x1FFF
		nesRam[address & 0x07FF] = data;
		break;
	case 1: // 0x2000-0x3FFF
		// TODO nsf
		nesPpuRegisters.write(address & 7, data);
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

u8 NesMapper::read(u16 address) {
	u8 data;
	switch (address >> 13) {
	case 0:	// 0x0000-0x1FFF
		data = nesRam[address & 0x07FF];
		break;
	case 1:	// 0x2000-0x3FFF
		data = nesPpuRegisters.read(address & 7);
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
		data = readDirect(address);
		break;
	}
	return data;
}

void NesMapper::writeLow(u16 address, u8 data) {
	if (address >= 0x6000) // < 0x8000
		nesCpuBanks[address >> 13][address & 0x1FFF] = data;
}

u8 NesMapper::readLow(u16 address) {
	if (address >= 0x6000) // < 0x8000
		return nesCpuBanks[address >> 13][address & 0x1FFF];
	return address >> 8;
}

void NesMapper::writeHigh(u16 address, u8 data) {
	Q_UNUSED(address)
	Q_UNUSED(data)
}

void NesMapper::writeReg(u16 address, u8 data) {
	if (address == 0x4014) {
		nesPpu.dma(data);
		nesCpu.dma(514);
	} else if (address == 0x4016) {
		nesPad.write(0, data);
	} else if (address == 0x4017) {
		nesApu.write(0x17, data);
		nesPad.write(1, data);
	} else if (address < 0x4017) {
		nesApu.write(address & 0x1F, data);
	} else {
		writeEx(address, data);
	}
}

u8 NesMapper::readReg(u16 address) {
	if (address == 0x4014) {
		return 0x14;
	} if (address == 0x4016) {
		u8 data = m_pad->read(0);
		return data | 0x40; // TODO | m_TapeOut
	} else if (address == 0x4017) {
		u8 data = m_pad->read(1);
		return data | m_apu->read(0x17);
	} else if (address < 0x4017) {
		return m_apu->read(address & 0x1F);
	} else {
		return readEx(address);
	}
}

void NesMapper::writeEx(u16 address, u8 data) {
	Q_UNUSED(address)
	Q_UNUSED(data)
}

u8 NesMapper::readEx(u16 address) {
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
	for (int i = 0; i < nesGameGenieList.size(); i++) {
		const GameGenieCode &code = nesGameGenieList.at(i);
		uint address = code.address() | 0x8000;
		if (readDirect(address) == code.replaceData())
			writeDirect(address, code.expectedData());
	}
	nesGameGenieList = codes;
	processGameGenieCodes();
}

static void processGameGenieCodes() {
	for (int i = 0; i < nesGameGenieList.size(); i++) {
		GameGenieCode &code = nesGameGenieList[i];
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

void NesMapper::setMirroring(NesMirroring mirroring) {
	if (mirroring == VerticalMirroring)
		setMirroring(0, 1, 0, 1);
	else if (mirroring == HorizontalMirroring)
		setMirroring(0, 0, 1, 1);
	else if (mirroring == SingleHigh)
		setMirroring(1, 1, 1, 1);
	else if (mirroring == SingleLow)
		setMirroring(0, 0, 0, 0);
	else if (mirroring == FourScreenMirroring)
		setMirroring(0, 1, 2, 3);
}

void NesMapper::setMirroring(uint bank0, uint bank1, uint bank2, uint bank3) {
	nesPpuBanks[8] = nesPpuBanks[12] = nesVram + bank0*0x400;
	nesPpuBanks[9] = nesPpuBanks[13] = nesVram + bank1*0x400;
	nesPpuBanks[10] = nesPpuBanks[14] = nesVram + bank2*0x400;
	nesPpuBanks[11] = nesPpuBanks[15] = nesVram + bank3*0x400;
}

void NesMapper::horizontalSync(int scanline)
{ Q_UNUSED(scanline) }

void NesMapper::verticalSync()
{ }

void NesMapper::addressBusLatch(u16 address)
{ Q_UNUSED(address) }

void NesMapper::characterLatch(u16 address)
{ Q_UNUSED(address) }

void NesMapper::extensionLatchX(uint x)
{ Q_UNUSED(x) }

void NesMapper::extensionLatch(u16 address, u8 *plane1, u8 *plane2, u8 *attribute) {
	Q_UNUSED(address)
	Q_UNUSED(plane1)
	Q_UNUSED(plane2)
	Q_UNUSED(attribute)
}

bool NesMapper::save(QDataStream &s) {
	// CPU
	for (int i = 0; i < 8; i++) {
		u8 *bank = nesCpuBanks[i];
		u8 type;
		uint offset;
		if (bank >= nesRom && bank < (nesRom + nesRomSize)) {
			type = 0;
			offset = bank - nesRom;
		} else if (bank >= nesRam && bank < (nesRam + sizeof(nesRam))) {
			type = 1;
			offset = bank - nesRam;
		} else if (bank >= nesWram && bank < (nesWram + sizeof(nesWram))) {
			type = 2;
			offset = bank - nesWram;
		} else if (bank >= nesXram && bank < (nesXram + sizeof(nesXram))) {
			type = 3;
			offset = bank - nesXram;
		} else {
			return false;
		}
		s << type;
		s << offset;
	}
	if (s.writeRawData(reinterpret_cast<const char *>(nesRam), sizeof(nesRam)) != sizeof(nesRam))
		return false;
	if (s.writeRawData(reinterpret_cast<const char *>(nesWram), sizeof(nesWram)) != sizeof(nesWram))
		return false;
	if (s.writeRawData(reinterpret_cast<const char *>(nesXram), sizeof(nesXram)) != sizeof(nesXram))
		return false;
	s << m_irqOut;
	// PPU
	if (s.writeRawData(reinterpret_cast<const char *>(nesVram), sizeof(nesVram)) != sizeof(nesVram))
		return false;
	if (s.writeRawData(reinterpret_cast<const char *>(nesCram), sizeof(nesCram)) != sizeof(nesCram))
		return false;
	for (int i = 0; i < 16; i++) {
		s << u8(nesPpuBanksType[i]);
		if (nesPpuBanksType[i] == VromBank)
			s << uint(nesPpuBanks[i] - nesVrom);
		else if (nesPpuBanksType[i] == CramBank)
			s << uint(nesPpuBanks[i] - nesCram);
		else if (nesPpuBanksType[i] == VramBank)
			s << uint(nesPpuBanks[i] - nesVram);
	}
	return true;
}

bool NesMapper::load(QDataStream &s) {
	// CPU
	for (int i = 0; i < 8; i++) {
		u8 type;
		uint offset;
		s >> type;
		s >> offset;
		if (type == 0)
			nesCpuBanks[i] = nesRom + offset;
		else if (type == 1)
			nesCpuBanks[i] = nesRam + offset;
		else if (type == 2)
			nesCpuBanks[i] = nesWram + offset;
		else if (type == 3)
			nesCpuBanks[i] = nesXram + offset;
		else
			return false;
	}
	if (s.readRawData(reinterpret_cast<char *>(nesRam), sizeof(nesRam)) != sizeof(nesRam))
		return false;
	if (s.readRawData(reinterpret_cast<char *>(nesWram), sizeof(nesWram)) != sizeof(nesWram))
		return false;
	if (s.readRawData(reinterpret_cast<char *>(nesXram), sizeof(nesXram)) != sizeof(nesXram))
		return false;
	s >> m_irqOut;
	// PPU
	if (s.readRawData(reinterpret_cast<char *>(nesVram), sizeof(nesVram)) != sizeof(nesVram))
		return false;
	if (s.readRawData(reinterpret_cast<char *>(nesCram), sizeof(nesCram)) != sizeof(nesCram))
		return false;
	for (int i = 0; i < 16; i++) {
		u8 bType;
		s >> bType;
		nesPpuBanksType[i] = static_cast<BankType>(bType);

		uint offset;
		s >> offset;
		if (nesPpuBanksType[i] == VromBank)
			nesPpuBanks[i] = nesVrom + offset;
		else if (nesPpuBanksType[i] == CramBank)
			nesPpuBanks[i] = nesCram + offset;
		else if (nesPpuBanksType[i] == VramBank)
			nesPpuBanks[i] = nesVram + offset;
		else
			return false;
	}
	return true;
}
