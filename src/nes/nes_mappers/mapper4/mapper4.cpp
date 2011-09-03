#include "mapper4.h"
#include "nesppu.h"
#include "nesdisk.h"
#include <QDebug>

class NesMapper4Data {
public:
	enum Irq {
		IrqNone = 0,
		IrqKlax,
		IrqShougimeikan,
		IrqDai2JiSuper,
		IrqDBZ2,
		IrqRockman3
	};

	quint8	reg[8];
	quint8	prg0, prg1;
	quint8	chr01, chr23, chr4, chr5, chr6, chr7;

	quint8	irq_type;
	quint8	irq_enable;
	quint8	irq_counter;
	quint8	irq_latch;
	quint8	irq_request;
	quint8	irq_preset;
	quint8	irq_preset_vbl;

	quint8	vs_patch;
	quint8	vs_index;
};

CpuMapper4::CpuMapper4(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
	d = new NesMapper4Data();
}

CpuMapper4::~CpuMapper4() {
	delete d;
}

void CpuMapper4::reset() {
	ppuMapper = static_cast<PpuMapper4 *>(mapper()->ppuMapper());

	qMemSet(d->reg, 0, sizeof(d->reg));
	d->prg0 = 0;
	d->prg1 = 1;
	updateBanks();

	d->irq_enable = 0;	// Disable
	d->irq_counter = 0;
	d->irq_latch = 0xFF;
	d->irq_request = 0;
	d->irq_preset = 0;
	d->irq_preset_vbl = 0;

	d->irq_type = NesMapper4Data::IrqNone;

	quint32 crc = disk()->crc();

	if (crc == 0x5c707ac4) {	// Mother(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xcb106f49) {	// F-1 Sensation(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x14a01c70) {	// Gun-Dec(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xeffeea40) {	// For Klax(J)
		d->irq_type = NesMapper4Data::IrqKlax;
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xc17ae2dc) {	// God Slayer - Haruka Tenkuu no Sonata(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x5a6860f1) {	// Shougi Meikan '92(J)
		d->irq_type = NesMapper4Data::IrqShougimeikan;
	}
	if (crc == 0xae280e20) {	// Shougi Meikan '93(J)
		d->irq_type = NesMapper4Data::IrqShougimeikan;
	}
	if (crc == 0xe19a2473) {	// Sugoro Quest - Dice no Senshi Tachi(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xa9a0d729) {	// Dai Kaijuu - Deburas(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xc5fea9f2) {	// Dai 2 Ji - Super Robot Taisen(J)
		d->irq_type = NesMapper4Data::IrqDai2JiSuper;
	}
	if (crc == 0xd852c2f7) {	// Time Zone(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xecfd3c69) {	// Taito Chase H.Q.(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xaafe699c) {	// Ninja Ryukenden 3 - Yomi no Hakobune(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x6cc62c06) {	// Hoshi no Kirby - Yume no Izumi no Monogatari(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x8685f366) {	// Matendouji(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x8635fed1) {	// Mickey Mouse 3 - Yume Fuusen(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x7c7ab58e) {	// Walkuere no Bouken - Toki no Kagi Densetsu(J)
		ppu()->setRenderMethod(NesPpu::PostRender);
	}
	if (crc == 0x26ff3ea2) {	// Yume Penguin Monogatari(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x126ea4a0) {	// Summer Carnival '92 Recca(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}

	if (crc == 0x1d2e5018		// Rockman 3(J)
	 || crc == 0x6b999aaf) {	// Megaman 3(U)
		d->irq_type = NesMapper4Data::IrqRockman3;
	}

	if (crc == 0xd88d48d7) {	// Kick Master(U)
		d->irq_type = NesMapper4Data::IrqRockman3;
	}

	if (crc == 0xA67EA466) {	// Alien 3(U)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}

	if (crc == 0xe763891b) {	// DBZ2
		d->irq_type = NesMapper4Data::IrqDBZ2;
	}

	// VS-Unisystem
	d->vs_patch = 0;
	d->vs_index = 0;

	if (crc == 0xeb2dba63		// VS TKO Boxing
	 || crc == 0x98cfe016) {	// VS TKO Boxing (Alt)
		d->vs_patch = 1;
	}
	if (crc == 0x135adf7c) {	// VS Atari RBI Baseball
		d->vs_patch = 2;
	}
	if (crc == 0xf9d3b0a3		// VS Super Xevious
	 || crc == 0x9924980a		// VS Super Xevious (b1)
	 || crc == 0x66bb838f) {	// VS Super Xevious (b2)
		d->vs_patch = 3;
	}
}

quint8 CpuMapper4::readLow(quint16 address) {
	if (!d->vs_patch) {
		if (address >= 0x5000 && address < 0x6000)
			return m_xram[address - 0x4000];
	} else {
		if (d->vs_patch == 1) {
			// VS TKO Boxing Security
			if (address == 0x5E00) {
				d->vs_index = 0;
				return 0x00;
			} else if (address == 0x5E01) {
				static quint8 vsTKOBoxingSecurity[32] = {
					0xFF, 0xBF, 0xB7, 0x97, 0x97, 0x17, 0x57, 0x4F,
					0x6F, 0x6B, 0xEB, 0xA9, 0xB1, 0x90, 0x94, 0x14,
					0x56, 0x4E, 0x6F, 0x6B, 0xEB, 0xA9, 0xB1, 0x90,
					0xD4, 0x5C, 0x3E, 0x26, 0x87, 0x83, 0x13, 0x00
				};
				return vsTKOBoxingSecurity[(d->vs_index++) & 0x1F];
			}
		} else if (d->vs_patch == 2) {
			// VS Atari RBI Baseball Security
			if (address == 0x5E00) {
				d->vs_index = 0;
				return 0x00;
			} else if (address == 0x5E01) {
				if (d->vs_index++ == 9)
					return 0x6F;
				else
					return 0xB4;
			}
		} else if (d->vs_patch == 3) {
			// VS Super Xevious
			switch (address) {
			case 0x54FF:
				return 0x05;
			case 0x5678:
				if (d->vs_index)
					return 0x00;
				else
					return 0x01;
				break;
			case 0x578F:
				if (d->vs_index)
					return 0xD1;
				else
					return 0x89;
				break;
			case 0x5567:
				if (d->vs_index) {
					d->vs_index = 0;
					return 0x3E;
				} else {
					d->vs_index = 1;
					return 0x37;
				}
				break;
			default:
				break;
			}
		}
	}
	return NesCpuMapper::readLow(address);
}

void CpuMapper4::writeLow(quint16 address, quint8 data) {
	if (address >= 0x5000 && address < 0x6000)
		m_xram[address - 0x4000] = data;
	else
		NesCpuMapper::writeLow(address, data);
}

void CpuMapper4::writeHigh(quint16 address, quint8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		d->reg[0] = data;
		updateBanks();
		ppuMapper->updateBanks();
		break;
	case 0x8001:
		d->reg[1] = data;
		switch (d->reg[0] & 0x07) {
		case 0x00: d->chr01 = data & 0xFE; ppuMapper->updateBanks(); break;
		case 0x01: d->chr23 = data & 0xFE; ppuMapper->updateBanks(); break;
		case 0x02: d->chr4 = data; ppuMapper->updateBanks(); break;
		case 0x03: d->chr5 = data; ppuMapper->updateBanks(); break;
		case 0x04: d->chr6 = data; ppuMapper->updateBanks(); break;
		case 0x05: d->chr7 = data; ppuMapper->updateBanks(); break;
		case 0x06: d->prg0 = data; updateBanks(); break;
		case 0x07: d->prg1 = data; updateBanks(); break;
		}
		break;
	case 0xA000:
		d->reg[2] = data;
		if (disk()->mirroring() != NesPpuMapper::FourScreen) {
			if (data & 0x01)
				ppuMapper->setMirroring(NesPpuMapper::Horizontal);
			else
				ppuMapper->setMirroring(NesPpuMapper::Vertical);
		}
		break;
	case 0xA001:
		d->reg[3] = data;
		break;
	case 0xC000:
		d->reg[4] = data;
		if (d->irq_type == NesMapper4Data::IrqKlax || d->irq_type == NesMapper4Data::IrqRockman3)
			d->irq_counter = data;
		else
			d->irq_latch = data;
		if (d->irq_type == NesMapper4Data::IrqDBZ2)
			d->irq_latch = 0x07;
		break;
	case 0xC001:
		d->reg[5] = data;
		if (d->irq_type == NesMapper4Data::IrqKlax || d->irq_type == NesMapper4Data::IrqRockman3) {
			d->irq_latch = data;
		} else {
			if ((ppu()->scanline() < NesPpu::VisibleScreenHeight) || (d->irq_type == NesMapper4Data::IrqShougimeikan)) {
				d->irq_counter |= 0x80;
				d->irq_preset = 0xFF;
			} else {
				d->irq_counter |= 0x80;
				d->irq_preset_vbl = 0xFF;
				d->irq_preset = 0;
			}
		}
		break;
	case 0xE000:
		d->reg[6] = data;
		d->irq_enable = 0;
		d->irq_request = 0;
		setIrqSignalOut(false);
		break;
	case 0xE001:
		d->reg[7] = data;
		d->irq_enable = 1;
		d->irq_request = 0;
		break;
	}
}

void CpuMapper4::updateBanks() {
	if (d->reg[0] & 0x40)
		setRom8KBanks(romSize8KB()-2, d->prg1, d->prg0, romSize8KB()-1);
	else
		setRom8KBanks(d->prg0, d->prg1, romSize8KB()-2, romSize8KB()-1);
}

PpuMapper4::PpuMapper4(NesMapper *mapper) :
	NesPpuMapper(mapper),
	d(0),
	cpuMapper(0),
	ppuRegisters(0) {
}

void PpuMapper4::reset() {
	cpuMapper = static_cast<CpuMapper4 *>(mapper()->cpuMapper());
	d = cpuMapper->d;
	ppuRegisters = cpuMapper->ppu()->registers();

	d->chr01 = 0;
	d->chr23 = 2;
	d->chr4  = 4;
	d->chr5  = 5;
	d->chr6  = 6;
	d->chr7  = 7;
	updateBanks();
}

void PpuMapper4::horizontalSync(int scanline) {
	if (d->irq_type == NesMapper4Data::IrqKlax) {
		if (scanline < NesPpu::VisibleScreenHeight && ppuRegisters->isDisplayOn()) {
			if (d->irq_enable) {
				if (d->irq_counter == 0) {
					d->irq_counter = d->irq_latch;
					d->irq_request = 0xFF;
				}
				if (d->irq_counter > 0)
					d->irq_counter--;
			}
		}
		if (d->irq_request)
			cpuMapper->setIrqSignalOut(true);
	} else if (d->irq_type == NesMapper4Data::IrqRockman3) {
		if (scanline < NesPpu::VisibleScreenHeight && ppuRegisters->isDisplayOn()) {
			if (d->irq_enable) {
				if (!(--d->irq_counter)) {
					d->irq_request = 0xFF;
					d->irq_counter = d->irq_latch;
				}
			}
		}
		if (d->irq_request)
			cpuMapper->setIrqSignalOut(true);
	} else {
		if (scanline < NesPpu::VisibleScreenHeight && ppuRegisters->isDisplayOn()) {
			if (d->irq_preset_vbl) {
				d->irq_counter = d->irq_latch;
				d->irq_preset_vbl = 0;
			}
			if (d->irq_preset) {
				d->irq_counter = d->irq_latch;
				d->irq_preset = 0;
				if (d->irq_type == NesMapper4Data::IrqDai2JiSuper && scanline == 0) {
					d->irq_counter--;
				}
			} else if (d->irq_counter > 0) {
				d->irq_counter--;
			}
			if (d->irq_counter == 0) {
				if (d->irq_enable) {
					d->irq_request = 0xFF;
					cpuMapper->setIrqSignalOut(true);
				}
				d->irq_preset = 0xFF;
			}
		}
	}
}

void PpuMapper4::updateBanks() {
	if (vromSize1KB()) {
		if (d->reg[0] & 0x80) {
			setVrom1KBank(4, d->chr01);
			setVrom1KBank(5, d->chr01+1);
			setVrom1KBank(6, d->chr23);
			setVrom1KBank(7, d->chr23+1);
			setVrom1KBank(0, d->chr4);
			setVrom1KBank(1, d->chr5);
			setVrom1KBank(2, d->chr6);
			setVrom1KBank(3, d->chr7);
		} else {
			setVrom1KBank(0, d->chr01);
			setVrom1KBank(1, d->chr01+1);
			setVrom1KBank(2, d->chr23);
			setVrom1KBank(3, d->chr23+1);
			setVrom1KBank(4, d->chr4);
			setVrom1KBank(5, d->chr5);
			setVrom1KBank(6, d->chr6);
			setVrom1KBank(7, d->chr7);
		}
	} else {
		if (d->reg[0] & 0x80) {
			setCram1KBank(4, (d->chr01+0)&0x07);
			setCram1KBank(5, (d->chr01+1)&0x07);
			setCram1KBank(6, (d->chr23+0)&0x07);
			setCram1KBank(7, (d->chr23+1)&0x07);
			setCram1KBank(0, d->chr4&0x07);
			setCram1KBank(1, d->chr5&0x07);
			setCram1KBank(2, d->chr6&0x07);
			setCram1KBank(3, d->chr7&0x07);
		} else {
			setCram1KBank(0, (d->chr01+0)&0x07);
			setCram1KBank(1, (d->chr01+1)&0x07);
			setCram1KBank(2, (d->chr23+0)&0x07);
			setCram1KBank(3, (d->chr23+1)&0x07);
			setCram1KBank(4, d->chr4&0x07);
			setCram1KBank(5, d->chr5&0x07);
			setCram1KBank(6, d->chr6&0x07);
			setCram1KBank(7, d->chr7&0x07);
		}
	}
}

bool CpuMapper4::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (uint i = 0; i < sizeof(d->reg); i++)
		s << d->reg[i];
	s << d->prg0 << d->prg1;
	s << d->chr01 << d->chr23 << d->chr4 << d->chr5 << d->chr6 << d->chr7;
	s << d->irq_enable;
	s << d->irq_counter;
	s << d->irq_latch;
	s << d->irq_request;
	s << d->irq_preset;
	s << d->irq_preset_vbl;
	return true;
}

bool CpuMapper4::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (uint i = 0; i < sizeof(d->reg); i++)
		s >> d->reg[i];
	s >> d->prg0 >> d->prg1;
	s >> d->chr01 >> d->chr23 >> d->chr4 >> d->chr5 >> d->chr6 >> d->chr7;
	s >> d->irq_enable;
	s >> d->irq_counter;
	s >> d->irq_latch;
	s >> d->irq_request;
	s >> d->irq_preset;
	s >> d->irq_preset_vbl;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(4, "Nintendo MMC3")