#include "mapper16.h"
#include "nesdisk.h"
#include "nesppu.h"
#include "nesmappereeprom.h"

class NesMapper16Data {
public:
	enum {
		IrqClock,
		IrqHSync
	};
	quint8 patch;	// For Famicom Jump 2
	quint8 eeprom_type;

	quint8 reg[3];

	quint8 irq_enable;
	int irq_counter;
	int irq_latch;
	quint8 irq_type;

	X24C01 x24c01;
	X24C02 x24c02;
};

CpuMapper16::CpuMapper16(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
	d = new NesMapper16Data();
}

CpuMapper16::~CpuMapper16() {
	delete d;
}

void CpuMapper16::reset() {
	ppuMapper = static_cast<PpuMapper16 *>(mapper()->ppuMapper());

	d->patch = 0;

	d->reg[0] = d->reg[1] = d->reg[2] = 0;
	d->irq_enable = 0;
	d->irq_counter = 0;
	d->irq_latch = 0;

	d->irq_type = NesMapper16Data::IrqClock;

	d->eeprom_type = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);

	quint32 crc = disk()->crc();

	if (crc == 0x3f15d20d		// Famicom Jump 2(J)
	 || crc == 0xf76aa523) {	// Famicom Jump 2(J)(alt)
		d->patch = 1;
		d->eeprom_type = 0xFF;

		m_wram[0x0BBC] = 0xFF;
	}

	if (crc == 0x1d6f27f7) {	// Dragon Ball Z 2(Korean Hack)
		d->irq_type = NesMapper16Data::IrqHSync;
		d->eeprom_type = 1;
	}
	if (crc == 0x6f7247c8) {	// Dragon Ball Z 3(Korean Hack)
		d->eeprom_type = 1;
	}

	if (crc == 0x6c6c2feb		// Dragon Ball 3 - Gokuu Den(J)
	 || crc == 0x8edeb257) {	// Dragon Ball 3 - Gokuu Den(J)(Alt)
	}
	if (crc == 0x31cd9903) {	// Dragon Ball Z - Kyoushuu! Saiya Jin(J)
		d->irq_type = NesMapper16Data::IrqHSync;
	}
	if (crc == 0xe49fc53e		// Dragon Ball Z 2 - Gekishin Freeza!!(J)
	 || crc == 0x1582fee0) {	// Dragon Ball Z 2 - Gekishin Freeza!!(J) [alt]
		d->irq_type = NesMapper16Data::IrqHSync;
		d->eeprom_type = 1;
	}
	if (crc == 0x09499f4d) {	// Dragon Ball Z 3 - Ressen Jinzou Ningen(J)
		d->irq_type = NesMapper16Data::IrqHSync;
		d->eeprom_type = 1;
	}
	if (crc == 0x2e991109) {	// Dragon Ball Z Gaiden - Saiya Jin Zetsumetsu Keikaku (J)
		d->irq_type = NesMapper16Data::IrqHSync;
		d->eeprom_type = 1;
	}

	if (crc == 0x73ac76db		// SD Gundam Gaiden - Knight Gundam Monogatari 2 - Hikari no Kishi(J)
	 || crc == 0x81a15eb8) {	// SD Gundam Gaiden - Knight Gundam Monogatari 3 - Densetsu no Kishi Dan(J)
		d->eeprom_type = 1;
	}
	if (crc == 0x170250de) {	// Rokudenashi Blues(J)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
		d->eeprom_type = 1;
	}

	if (crc == 0x0be0a328 		// Datach - SD Gundam - Gundam Wars(J)
	 || crc == 0x19e81461		// Datach - Dragon Ball Z - Gekitou Tenkaichi Budou Kai(J)
	 || crc == 0x5b457641		// Datach - Ultraman Club - Supokon Fight!(J)
	 || crc == 0x894efdbc		// Datach - Crayon Shin Chan - Ora to Poi Poi(J)
	 || crc == 0x983d8175		// Datach - Battle Rush - Build Up Robot Tournament(J)
	 || crc == 0xbe06853f) {	// Datach - J League Super Top Players(J)
		d->eeprom_type = 2;
	}
	if (crc == 0xf51a7f46) {	// Datach - Yuu Yuu Hakusho - Bakutou Ankoku Bujutsu Kai(J)
		d->irq_type = NesMapper16Data::IrqHSync;
		d->eeprom_type = 2;
	}

	if (d->eeprom_type == 0) {
		d->x24c01.reset(m_wram);
	} else if (d->eeprom_type == 1) {
		d->x24c02.reset(m_wram);
	} else if (d->eeprom_type == 2) {
		d->x24c02.reset(m_wram);
		d->x24c01.reset(m_wram+256);
	}
	setIrqSignalOut(false);
}

quint8 CpuMapper16::readLow(quint16 address) {
	if (d->patch) {
		return NesCpuMapper::readLow(address);
	} else {
		if ((address & 0x00FF) == 0x0000) {
			bool ret = false;
			if (d->eeprom_type == 0) {
				ret = d->x24c01.read();
			} else if (d->eeprom_type == 1) {
				ret = d->x24c02.read();
			} else if (d->eeprom_type == 2) {
				ret = d->x24c02.read() && d->x24c01.read();
			}
			return	(ret?0x10:0);// TODO barcode |(nes->GetBarcodeStatus());
		}
	}
	return 0x00;
}

void CpuMapper16::writeLow(quint16 address, quint8 data) {
	if (!d->patch) {
		writeSubA(address, data);
	} else {
		NesCpuMapper::writeLow(address, data);
	}
}

void CpuMapper16::writeHigh(quint16 address, quint8 data) {
	if (!d->patch) {
		writeSubA(address, data);
	} else {
		writeSubB(address, data);
	}
}
void CpuMapper16::writeSubA(quint16 address, quint8 data) {
	switch(address & 0x000F) {
	case 0x0000:
	case 0x0001:
	case 0x0002:
	case 0x0003:
	case 0x0004:
	case 0x0005:
	case 0x0006:
	case 0x0007:
		if (ppuMapper->vromSize1KB())
			ppuMapper->setVrom1KBank(address&0x0007, data);
		if (d->eeprom_type == 2) {
			d->reg[0] = data;
			d->x24c01.write(data&0x08, d->reg[1]&0x40);
		}
		break;

	case 0x0008:
		setRom16KBank(4, data);
		break;

	case 0x0009:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 3));
		break;

	case 0x000A:
		d->irq_enable = data & 0x01;
		d->irq_counter = d->irq_latch;
		setIrqSignalOut(false);
		break;
	case 0x000B:
		d->irq_latch = (d->irq_latch & 0xFF00) | data;
		d->irq_counter = (d->irq_counter & 0xFF00) | data;
		break;
	case 0x000C:
		d->irq_latch = (data << 8) | (d->irq_latch & 0x00FF);
		d->irq_counter = (data << 8) | (d->irq_counter & 0x00FF);
		break;

	case 0x000D:
		// EEPTYPE0(DragonBallZ)
		if (d->eeprom_type == 0) {
			d->x24c01.write(data&0x20, data&0x40);
		}
		// EEPTYPE1(DragonBallZ2,Z3,Z Gaiden)
		if (d->eeprom_type == 1) {
			d->x24c02.write(data&0x20, data&0x40);
		}
		// EEPTYPE2(DATACH)
		if (d->eeprom_type == 2) {
			d->reg[1] = data;
			d->x24c02.write(data&0x20, data&0x40);
			d->x24c01.write(d->reg[0]&0x08, data&0x40);
		}
		break;
	}
}

// Famicom Jump 2
void CpuMapper16::writeSubB(quint16 addr, quint8 data)
{
	switch(addr) {
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		d->reg[0] = data & 0x01;
		setRom8KBank(4, d->reg[0]*0x20+d->reg[2]*2+0);
		setRom8KBank(5, d->reg[0]*0x20+d->reg[2]*2+1);
		break;
	case 0x8004:
	case 0x8005:
	case 0x8006:
	case 0x8007:
		d->reg[1] = data & 0x01;
		setRom8KBank(6, d->reg[1]*0x20+0x1E);
		setRom8KBank(7, d->reg[1]*0x20+0x1F);
		break;
	case 0x8008:
		d->reg[2] = data;
		setRom8KBank(4, d->reg[0]*0x20+d->reg[2]*2+0);
		setRom8KBank(5, d->reg[0]*0x20+d->reg[2]*2+1);
		setRom8KBank(6, d->reg[1]*0x20+0x1E);
		setRom8KBank(7, d->reg[1]*0x20+0x1F);
		break;

	case 0x8009:
		ppuMapper->setMirroring(static_cast<NesPpuMapper::Mirroring>(data & 3));
		break;

	case 0x800A:
		d->irq_enable = data & 0x01;
		d->irq_counter = d->irq_latch;
		setIrqSignalOut(false);
		break;
	case 0x800B:
		d->irq_latch = (d->irq_latch & 0xFF00) | data;
		break;
	case 0x800C:
		d->irq_latch = (data << 8) | (d->irq_latch & 0x00FF);
		break;

	case 0x800D:
		break;
	}
}

void CpuMapper16::clock(uint cycles) {
	if (d->irq_enable && d->irq_type == NesMapper16Data::IrqClock) {
		if ((d->irq_counter-=cycles) <= 0) {
			setIrqSignalOut(true);
			d->irq_counter &= 0xFFFF;
		}
	}
}

PpuMapper16::PpuMapper16(NesMapper *mapper) :
	NesPpuMapper(mapper),
	d(0),
	cpuMapper(0) {
}

void PpuMapper16::reset() {
	cpuMapper = static_cast<CpuMapper16 *>(mapper()->cpuMapper());
	d = cpuMapper->d;
}

void PpuMapper16::horizontalSync(int scanline) {
	Q_UNUSED(scanline)
	if (d->irq_enable && d->irq_type == NesMapper16Data::IrqHSync) {
		if (d->irq_counter <= 113) {
			cpuMapper->setIrqSignalOut(true);
			d->irq_counter &= 0xFFFF;
		} else {
			d->irq_counter -= 113;
		}
	}
}

bool CpuMapper16::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	if (!d->x24c01.save(s))
		return false;
	if (!d->x24c02.save(s))
		return false;
	s << d->reg[0];
	s << d->reg[1];
	s << d->reg[2];
	s << d->irq_enable;
	s << d->irq_counter;
	s << d->irq_latch;
	return true;
}

bool CpuMapper16::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	if (!d->x24c01.load(s))
		return false;
	if (!d->x24c02.load(s))
		return false;
	s >> d->reg[0];
	s >> d->reg[1];
	s >> d->reg[2];
	s >> d->irq_enable;
	s >> d->irq_counter;
	s >> d->irq_latch;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(16, "Bandai Standard")
