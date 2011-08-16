#include "mapper33.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDataStream>

CpuMapper33::CpuMapper33(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
}

void CpuMapper33::reset() {
	ppuMapper = mapper()->ppuMapper();

	reg[0] = 0;
	reg[1] = 2;
	reg[2] = 4;
	reg[3] = 5;
	reg[4] = 6;
	reg[5] = 7;
	reg[6] = 1;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;

	setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	if (ppuMapper->vromSize8KB())
		updateBanks();

	quint32 crc = disk()->crc();
	// Check For Old #33 games.... (CRC code by NesToy)
	if (crc == 0x5e9bc161		// Akira(J)
	 || crc == 0xecdbafa4		// Bakushou!! Jinsei Gekijou(J)
	 || crc == 0x59cd0c31		// Don Doko Don(J)
	 || crc == 0x837c1342		// Golf Ko Open(J)
	 || crc == 0x42d893e4		// Operation Wolf(J)
	 || crc == 0x1388aeb9		// Operation Wolf(U)
	 || crc == 0x07ee6d8f		// Power Blazer(J)
	 || crc == 0x5193fb54		// Takeshi no Sengoku Fuuunji(J)
	 || crc == 0xa71c3452) {	// Insector X(J)
		patch = 1;
	}


	ppu()->setRenderMethod(NesPpu::PreRender);

	if (crc == 0x202df297) {	// Captain Saver(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x63bb86b5) {	// The Jetsons(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
}

void CpuMapper33::writeHigh(quint16 address, quint8 data) {
	switch (address) {
	case 0x8000:
		if (patch) {
			if (data & 0x40)
				ppuMapper->setMirroring(NesPpuMapper::Horizontal);
			else
				ppuMapper->setMirroring(NesPpuMapper::Vertical);
			setRom8KBank(4, data & 0x1F);
		} else {
			setRom8KBank(4, data);
		}
		break;
	case 0x8001:
		if (patch) {
			setRom8KBank(5, data & 0x1F);
		} else {
			setRom8KBank(5, data);
		}
		break;

	case 0x8002:
		reg[0] = data;
		updateBanks();
		break;
	case 0x8003:
		reg[1] = data;
		updateBanks();
		break;
	case 0xA000:
		reg[2] = data;
		updateBanks();
		break;
	case 0xA001:
		reg[3] = data;
		updateBanks();
		break;
	case 0xA002:
		reg[4] = data;
		updateBanks();
		break;
	case 0xA003:
		reg[5] = data;
		updateBanks();
		break;

	case 0xC000:
		irq_latch = data;
		irq_counter = irq_latch;
		break;
	case 0xC001:
		irq_counter = irq_latch;
		break;
	case 0xC002:
		irq_enable = 1;
		break;
	case 0xC003:
		irq_enable = 0;
		break;

	case 0xE001:
	case 0xE002:
	case 0xE003:
		break;
	case 0xE000:
		if (data & 0x40)
			ppuMapper->setMirroring(NesPpuMapper::Horizontal);
		else
			ppuMapper->setMirroring(NesPpuMapper::Vertical);
		break;
	}
}

void CpuMapper33::updateBanks() {
	ppuMapper->setVrom2KBank(0, reg[0]);
	ppuMapper->setVrom2KBank(2, reg[1]);

	ppuMapper->setVrom1KBank(4, reg[2]);
	ppuMapper->setVrom1KBank(5, reg[3]);
	ppuMapper->setVrom1KBank(6, reg[4]);
	ppuMapper->setVrom1KBank(7, reg[5]);
}

PpuMapper33::PpuMapper33(NesMapper *mapper) :
	NesPpuMapper(mapper),
	cpuMapper(0) {
}

void PpuMapper33::reset() {
	cpuMapper = static_cast<CpuMapper33 *>(mapper()->cpuMapper());
}

void PpuMapper33::horizontalSync(int scanline) {
	if (scanline < NesPpu::VisibleScreenHeight && cpuMapper->ppu()->registers()->isDisplayOn()) {
		if (cpuMapper->irq_enable) {
			if (++cpuMapper->irq_counter == 0) {
				cpuMapper->irq_enable  = 0;
				cpuMapper->irq_counter = 0;
				cpuMapper->setIrqSignalOut(true);
			}
		}
	}
}

bool CpuMapper33::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < 7; i++)
		s << reg[i];
	s << irq_enable;
	s << irq_counter;
	s << irq_latch;
	return true;
}

bool CpuMapper33::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < 7; i++)
		s >> reg[i];
	s >> irq_enable;
	s >> irq_counter;
	s >> irq_latch;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(33, "Taito TC0190")
