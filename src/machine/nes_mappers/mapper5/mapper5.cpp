#include "mapper5.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

class NesMapper5Data {
public:
	enum Irq {
		IrqNone = 0,
		IrqMetal
	};
	quint8	sram_size;

	quint8	prg_size;		// $5100
	quint8	chr_size;		// $5101
	quint8	sram_we_a, sram_we_b;	// $5102-$5103
	quint8	graphic_mode;		// $5104
	quint8	nametable_mode;		// $5105
	quint8	nametable_type[4];	// $5105 use

	quint8	sram_page;		// $5113

	quint8	fill_chr, fill_pal;	// $5106-$5107
	quint8	split_control;		// $5200
	quint8	split_scroll;		// $5201
	quint8	split_page;		// $5202

	quint8	split_x;
	quint16	split_addr;
	quint16	split_yofs;

	quint8	chr_type;
	quint8	chr_mode;		// $5120-$512B use
	quint8	chr_page[2][8];		// $5120-$512B
	quint8 *BG_MEM_BANK[8];

	quint8	irq_status;		// $5204(R)
	quint8	irq_enable;		// $5204(W)
	quint8	irq_line;		// $5203
	quint8	irq_scanline;
	quint8	irq_clear;		// HSync
	quint8	irq_type;

	quint8	mult_a, mult_b;		// $5205-$5206

	bool cpu_bank_wren[8];
};

CpuMapper5::CpuMapper5(NesMapper *mapper) :
	NesCpuMapper(mapper),
	ppuMapper(0) {
	d = new NesMapper5Data();
}

CpuMapper5::~CpuMapper5() {
	delete d;
}

void CpuMapper5::reset() {
	ppuMapper = static_cast<PpuMapper5 *>(mapper()->ppuMapper());

	d->cpu_bank_wren[0] = true;
	d->cpu_bank_wren[1] = true;
	d->cpu_bank_wren[2] = true;
	d->cpu_bank_wren[3] = true;
	d->cpu_bank_wren[4] = false;
	d->cpu_bank_wren[5] = false;
	d->cpu_bank_wren[6] = false;
	d->cpu_bank_wren[7] = false;

	d->prg_size = 3;
	d->chr_size = 3;

	d->sram_we_a = 0x00;
	d->sram_we_b = 0x00;

	d->graphic_mode = 0;
	d->nametable_mode = 0;

	for (int i = 0; i < sizeof(d->nametable_type); i++)
		d->nametable_type[i] = 0;

	d->fill_chr = d->fill_pal = 0;
	d->split_control = d->split_scroll = d->split_page = 0;

	d->irq_enable = 0;
	d->irq_status = 0;
	d->irq_scanline = 0;
	d->irq_line = 0;
	d->irq_clear = 0;

	d->irq_type = NesMapper5Data::IrqNone;

	d->mult_a = d->mult_b = 0;

	d->chr_type = 0;
	d->chr_mode = 0;

	for (int i = 0; i < 8; i++) {
		d->chr_page[0][i] = i;
		d->chr_page[1][i] = 4+(i&0x03);
	}
	for (int i = 4; i < 8; i++)
		setRom8KBank(i, romSize8KB()-1);
	setBankSram(3, 0);

	d->sram_size = 0;

	quint32 crc = disk()->crc();

	if (crc == 0x2b548d75	// Bandit Kings of Ancient China(U)
	 || crc == 0xf4cd4998	// Dai Koukai Jidai(J)
	 || crc == 0x8fa95456	// Ishin no Arashi(J)
	 || crc == 0x98c8e090	// Nobunaga no Yabou - Sengoku Gunyuu Den(J)
	 || crc == 0x8e9a5e2f	// L'Empereur(Alt)(U)
	 || crc == 0x57e3218b	// L'Empereur(U)
	 || crc == 0x2f50bd38	// L'Empereur(J)
	 || crc == 0xb56958d1	// Nobunaga's Ambition 2(U)
	 || crc == 0xe6c28c5f	// Suikoden - Tenmei no Chikai(J)
	 || crc == 0xcd35e2e9) {	// Uncharted Waters(U)
		d->sram_size = 1;
	} else
	if (crc == 0xf4120e58	// Aoki Ookami to Shiroki Mejika - Genchou Hishi(J)
	 || crc == 0x286613d8	// Nobunaga no Yabou - Bushou Fuuun Roku(J)
	 || crc == 0x11eaad26	// Romance of the Three Kingdoms 2(U)
	 || crc == 0x95ba5733) {	// Sangokushi 2(J)
		d->sram_size = 2;
	}

	if (crc == 0x95ca9ec7) { // Castlevania 3 - Dracula's Curse(U)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}

	if (crc == 0xcd9acf43) { // Metal Slader Glory(J)
		d->irq_type = NesMapper5Data::IrqMetal;
	}

	if (crc == 0xe91548d8) { // Shin 4 Nin Uchi Mahjong - Yakuman Tengoku(J)
		d->chr_type = 1;
	}

	ppu()->setExternalLatchEnabled(true);
// TODO ex sound	nes->apu->SelectExSound( 8);
}

quint8 CpuMapper5::readLow(quint16 address) {
	quint8 data = address >> 8;

	switch (address) {
	case 0x5015:
		// TODO ex souund data = nes->apu->ExRead( address);
		break;

	case 0x5204:
		data = d->irq_status;
//		d->irq_status = 0;
		d->irq_status &= ~0x80;
		setIrqSignalOut(false);
		break;
	case 0x5205:
		data = d->mult_a*d->mult_b;
		break;
	case 0x5206:
		data = (quint8)(((quint16)d->mult_a*(quint16)d->mult_b)>>8);
		break;
	}

	if (address >= 0x5C00 && address <= 0x5FFF) {
		if (d->graphic_mode >= 2) { // ExRAM mode
			data = ppuMapper->m_vram[0x0800+(address&0x3FF)];
		}
	} else if (address >= 0x6000 && address <= 0x7FFF) {
		data = NesCpuMapper::readLow(address);
	}
	return data;
}

void CpuMapper5::writeLow(quint16 address, quint8 data) {
	switch (address) {
	case 0x5100:
		d->prg_size = data & 0x03;
		break;
	case 0x5101:
		d->chr_size = data & 0x03;
		break;

	case 0x5102:
		d->sram_we_a = data & 0x03;
		break;
	case 0x5103:
		d->sram_we_b = data & 0x03;
		break;

	case 0x5104:
		d->graphic_mode = data & 0x03;
		break;
	case 0x5105:
		d->nametable_mode = data;
		for (int i = 0; i < 4; i++) {
			d->nametable_type[i] = data&0x03;
			ppuMapper->setVram1KBank(8+i, d->nametable_type[i]);
			data >>= 2;
		}
		break;

	case 0x5106:
		d->fill_chr = data;
		break;
	case 0x5107:
		d->fill_pal = data & 0x03;
		break;

	case 0x5113:
		setBankSram(3, data&0x07);
		break;

	case 0x5114:
	case 0x5115:
	case 0x5116:
	case 0x5117:
		setBank(address, data);
		break;

	case 0x5120:
	case 0x5121:
	case 0x5122:
	case 0x5123:
	case 0x5124:
	case 0x5125:
	case 0x5126:
	case 0x5127:
		d->chr_mode = 0;
		d->chr_page[0][address&0x07] = data;
		ppuMapper->updateBanks();
		break;

	case 0x5128:
	case 0x5129:
	case 0x512A:
	case 0x512B:
		d->chr_mode = 1;
		d->chr_page[1][(address&0x03)+0] = data;
		d->chr_page[1][(address&0x03)+4] = data;
		ppuMapper->updateBanks();
		break;

	case 0x5200:
		d->split_control = data;
		break;
	case 0x5201:
		d->split_scroll = data;
		break;
	case 0x5202:
		d->split_page = data&0x3F;
		break;

	case 0x5203:
		d->irq_line = data;
		setIrqSignalOut(false);
		break;
	case 0x5204:
		d->irq_enable = data;
		setIrqSignalOut(false);
		break;

	case 0x5205:
		d->mult_a = data;
		break;
	case 0x5206:
		d->mult_b = data;
		break;

	default:
		if (address >= 0x5000 && address <= 0x5015) {
			// TODO ex apu nes->apu->ExWrite( address, data);
		} else if (address >= 0x5C00 && address <= 0x5FFF) {
			if (d->graphic_mode == 2) {		// ExRAM
				ppuMapper->m_vram[0x0800+(address&0x3FF)] = data;
			} else if (d->graphic_mode != 3) {	// Split,ExGraphic
				if (d->irq_status&0x40) {
					ppuMapper->m_vram[0x0800+(address&0x3FF)] = data;
				} else {
					ppuMapper->m_vram[0x0800+(address&0x3FF)] = 0;
				}
			}
		} else if (address >= 0x6000 && address <= 0x7FFF) {
			if ((d->sram_we_a == 0x02) && (d->sram_we_b == 0x01)) {
				if (d->cpu_bank_wren[3]) {
					writeDirect(address, data);
				}
			}
		}
		break;
	}
}

void CpuMapper5::writeHigh(quint16 address, quint8 data) {
	if (d->sram_we_a == 0x02 && d->sram_we_b == 0x01) {
		if (address >= 0x8000 && address < 0xE000) {
			if (d->cpu_bank_wren[address >> 13]) {
				writeDirect(address, data);
			}
		}
	}
}

void CpuMapper5::setBank(quint16 address, quint8 data) {
	if (data & 0x80) {
		// PROM Bank
		switch (address & 7) {
		case 4:
			if (d->prg_size == 3) {
				setRom8KBank(4, data&0x7F);
				d->cpu_bank_wren[4] = false;
			}
			break;
		case 5:
			if (d->prg_size == 1 || d->prg_size == 2) {
				setRom16KBank(4, (data&0x7F)>>1);
				d->cpu_bank_wren[4] = false;
				d->cpu_bank_wren[5] = false;
			} else if (d->prg_size == 3) {
				setRom8KBank(5, (data&0x7F));
				d->cpu_bank_wren[5] = false;
			}
			break;
		case 6:
			if (d->prg_size == 2 || d->prg_size == 3) {
				setRom8KBank(6, (data&0x7F));
				d->cpu_bank_wren[6] = false;
			}
			break;
		case 7:
			if (d->prg_size == 0) {
				setRom32KBank((data&0x7F)>>2);
				d->cpu_bank_wren[4] = false;
				d->cpu_bank_wren[5] = false;
				d->cpu_bank_wren[6] = false;
				d->cpu_bank_wren[7] = false;
			} else if (d->prg_size == 1) {
				setRom16KBank(6, (data&0x7F)>>1);
				d->cpu_bank_wren[6] = false;
				d->cpu_bank_wren[7] = false;
			} else if (d->prg_size == 2 || d->prg_size == 3) {
				setRom8KBank(7, (data&0x7F));
				d->cpu_bank_wren[7] = false;
			}
			break;
		}
	} else {
		// WRAM Bank
		switch (address & 7) {
		case 4:
			if (d->prg_size == 3) {
				setBankSram(4, data&0x07);
			}
			break;
		case 5:
			if (d->prg_size == 1 || d->prg_size == 2) {
				setBankSram(4, (data&0x06)+0);
				setBankSram(5, (data&0x06)+1);
			} else if (d->prg_size == 3) {
				setBankSram(5, data&0x07);
			}
			break;
		case 6:
			if (d->prg_size == 2 || d->prg_size == 3) {
				setBankSram(6, data&0x07);
			}
			break;
		}
	}
}

void CpuMapper5::setBankSram(quint8 page, quint8 data) {
	if (d->sram_size == 0) data = (data > 3) ? 8 : 0;
	if (d->sram_size == 1) data = (data > 3) ? 1 : 0;
	if (d->sram_size == 2) data = (data > 3) ? 8 : data;
	if (d->sram_size == 3) data = (data > 3) ? 4 : data;

	if (data != 8) {
		setWram8KBank(page, data);
		d->cpu_bank_wren[page] = true;
	} else {
		d->cpu_bank_wren[page] = false;
	}
}

PpuMapper5::PpuMapper5(NesMapper *mapper) :
	NesPpuMapper(mapper),
	d(0),
	cpuMapper(0),
	ppu(0),
	ppuRegisters(0) {
}

void PpuMapper5::reset() {
	cpuMapper = static_cast<CpuMapper5 *>(mapper()->cpuMapper());
	d = cpuMapper->d;
	ppu = cpuMapper->ppu();
	ppuRegisters = ppu->registers();

	setVrom8KBank(0);

	for (int i = 0; i < 8; i++) {
		d->BG_MEM_BANK[i] = m_vrom+0x0400*i;
	}

}

void PpuMapper5::updateBanks() {
	if (d->chr_mode == 0) {
		// PPU SP Bank
		switch (d->chr_size) {
		case 0:
			setVrom8KBank(d->chr_page[0][7]);
			break;
		case 1:
			setVrom4KBank(0, d->chr_page[0][3]);
			setVrom4KBank(4, d->chr_page[0][7]);
			break;
		case 2:
			setVrom2KBank(0, d->chr_page[0][1]);
			setVrom2KBank(2, d->chr_page[0][3]);
			setVrom2KBank(4, d->chr_page[0][5]);
			setVrom2KBank(6, d->chr_page[0][7]);
			break;
		case 3:
			for (int i = 0; i < 8; i++)
				setVrom1KBank(i, d->chr_page[0][i]);
			break;
		}
	} else if (d->chr_mode == 1) {
		// PPU BG Bank
		switch (d->chr_size) {
		case 0:
			for (int i = 0; i < 8; i++) {
				d->BG_MEM_BANK[i] = m_vrom+0x2000*(d->chr_page[1][7]%vromSize8KB())+0x0400*i;
			}
			break;
		case 1:
			for (int i = 0; i < 4; i++) {
				d->BG_MEM_BANK[i+0] = m_vrom+0x1000*(d->chr_page[1][3]%vromSize4KB())+0x0400*i;
				d->BG_MEM_BANK[i+4] = m_vrom+0x1000*(d->chr_page[1][7]%vromSize4KB())+0x0400*i;
			}
			break;
		case 2:
			for (int i = 0; i < 2; i++) {
				d->BG_MEM_BANK[i+0] = m_vrom+0x0800*(d->chr_page[1][1]%vromSize2KB())+0x0400*i;
				d->BG_MEM_BANK[i+2] = m_vrom+0x0800*(d->chr_page[1][3]%vromSize2KB())+0x0400*i;
				d->BG_MEM_BANK[i+4] = m_vrom+0x0800*(d->chr_page[1][5]%vromSize2KB())+0x0400*i;
				d->BG_MEM_BANK[i+6] = m_vrom+0x0800*(d->chr_page[1][7]%vromSize2KB())+0x0400*i;
			}
			break;
		case 3:
			for (int i = 0; i < 8; i++) {
				d->BG_MEM_BANK[i] = m_vrom+0x0400*(d->chr_page[1][i]%vromSize1KB());
			}
			break;
		}
	}
}

void PpuMapper5::horizontalSync(int scanline) {
	if (d->irq_type & NesMapper5Data::IrqMetal) {
		if (d->irq_scanline == d->irq_line) {
			d->irq_status |= 0x80;
		}
	}
	if (ppuRegisters->isDisplayOn() && scanline < NesPpu::VisibleScreenHeight) {
		d->irq_scanline++;
		d->irq_status |= 0x40;
		d->irq_clear = 0;
	} else if (d->irq_type & NesMapper5Data::IrqMetal) {
		d->irq_scanline = 0;
		d->irq_status &= ~0x80;
		d->irq_status &= ~0x40;
	}

	if (!(d->irq_type & NesMapper5Data::IrqMetal)) {
		if (d->irq_scanline == d->irq_line) {
			d->irq_status |= 0x80;
		}

		if (++d->irq_clear > 2) {
			d->irq_scanline = 0;
			d->irq_status &= ~0x80;
			d->irq_status &= ~0x40;
			cpuMapper->setIrqSignalOut(false);
		}
	}

	if ((d->irq_enable & 0x80) && (d->irq_status & 0x80) && (d->irq_status & 0x40)) {
		cpuMapper->setIrqSignalOut(true);
///		nes->cpu->IRQ_NotPending();
	}

	// For Split mode!
	if (scanline == 0) {
		d->split_yofs = d->split_scroll&0x07;
		d->split_addr = ((d->split_scroll&0xF8)<<2);
	} else if (ppuRegisters->isDisplayOn()) {
		if (d->split_yofs == 7) {
			d->split_yofs = 0;
			if ((d->split_addr & 0x03E0) == 0x03A0) {
				d->split_addr &= 0x001F;
			} else {
				if ((d->split_addr & 0x03E0) == 0x03E0) {
					d->split_addr &= 0x001F;
				} else {
					d->split_addr += 0x0020;
				}
			}
		} else {
			d->split_yofs++;
		}
	}
}

void PpuMapper5::extensionLatchX(uint x) {
	d->split_x = x;
}

void PpuMapper5::extensionLatch(quint16 address, quint8 *plane1, quint8 *plane2, quint8 *attribute) {
	quint16	ntbladr, attradr, tileadr, tileofs;
	quint16	tile_yofs;
	uint tilebank;
	bool bSplit;

	tile_yofs = ppu->scrollTileYOffset();

	bSplit = false;
	if (d->split_control & 0x80) {
		if (!(d->split_control&0x40)) {
			// Left side
			if ((d->split_control&0x1F) > d->split_x) {
				bSplit = true;
			}
		} else {
			// Right side
			if ((d->split_control&0x1F) <= d->split_x) {
				bSplit = true;
			}
		}
	}

	if (!bSplit) {
		if (d->nametable_type[(address&0x0C00)>>10] == 3) {
			// Fill mode
			if (d->graphic_mode == 1) {
				// ExGraphic mode
				ntbladr = 0x2000+(address&0x0FFF);
				// Get Nametable
				tileadr = d->fill_chr*0x10+tile_yofs;
				// Get TileBank
				tilebank = 0x1000*((m_vram[0x0800+(ntbladr&0x03FF)]&0x3F)%vromSize4KB());
				// Attribute
				*attribute = (d->fill_pal<<2)&0x0C;
				// Get Pattern
				*plane1 = m_vrom[tilebank+tileadr  ];
				*plane2 = m_vrom[tilebank+tileadr+8];
			} else {
				// Normal
				tileofs = ppu->tilePageOffset();
				tileadr = tileofs+d->fill_chr*0x10+tile_yofs;
				*attribute = (d->fill_pal<<2)&0x0C;
				// Get Pattern
				if (d->chr_type) {
					*plane1 = read(tileadr + 0);
					*plane2 = read(tileadr + 8);
				} else {
					*plane1 = d->BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
					*plane2 = d->BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
				}
			}
		} else if (d->graphic_mode == 1) {
			// ExGraphic mode
			ntbladr = 0x2000+(address&0x0FFF);
			// Get Nametable
			tileadr = (quint16)read(ntbladr)*0x10+tile_yofs;
			// Get TileBank
			tilebank = 0x1000*((m_vram[0x0800+(ntbladr&0x03FF)]&0x3F)%vromSize4KB());
			// Get Attribute
			*attribute = (m_vram[0x0800+(ntbladr&0x03FF)]&0xC0)>>4;
			// Get Pattern
			*plane1 = m_vrom[tilebank+tileadr  ];
			*plane2 = m_vrom[tilebank+tileadr+8];
		} else {
			// Normal or ExVRAM
			tileofs = ppu->tilePageOffset();
			ntbladr = 0x2000+(address&0x0FFF);
			attradr = 0x23C0+(address&0x0C00)+((address&0x0380)>>4)+((address&0x001C)>>2);
			// Get Nametable
			tileadr = tileofs+read(ntbladr)*0x10+tile_yofs;
			// Get Attribute
			*attribute = read(attradr);
			if (ntbladr & 0x0002) *attribute >>= 2;
			if (ntbladr & 0x0040) *attribute >>= 4;
			*attribute = (*attribute&0x03)<<2;
			// Get Pattern
			if (d->chr_type) {
				*plane1 = read(tileadr + 0);
				*plane2 = read(tileadr + 8);
			} else {
				*plane1 = d->BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
				*plane2 = d->BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			}
		}
	} else {
		ntbladr = ((d->split_addr&0x03E0)|(d->split_x&0x1F))&0x03FF;
		// Get Split TileBank
		tilebank = 0x1000*((int)d->split_page%vromSize4KB());
		tileadr  = (quint16)m_vram[0x0800+ntbladr]*0x10+d->split_yofs;
		// Get Attribute
		attradr = 0x03C0+((ntbladr&0x0380)>>4)+((ntbladr&0x001C)>>2);
		*attribute = m_vram[0x0800+attradr];
		if (ntbladr & 0x0002) *attribute >>= 2;
		if (ntbladr & 0x0040) *attribute >>= 4;
		*attribute = (*attribute&0x03)<<2;
		// Get Pattern
		*plane1 = m_vrom[tilebank+tileadr  ];
		*plane2 = m_vrom[tilebank+tileadr+8];
	}
}

bool CpuMapper5::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	s << d->prg_size;
	s << d->chr_size;
	s << d->sram_we_a;
	s << d->sram_we_b;
	s << d->graphic_mode;
	s << d->nametable_mode;
	s << d->nametable_type[0];
	s << d->nametable_type[1];
	s << d->nametable_type[2];
	s << d->nametable_type[3];
	s << d->sram_page;
	s << d->fill_chr;
	s << d->fill_pal;
	s << d->split_control;
	s << d->split_scroll;
	s << d->split_page;
	s << d->chr_mode;
	s << d->irq_status;
	s << d->irq_enable;
	s << d->irq_line;
	s << d->irq_scanline;
	s << d->irq_clear;
	s << d->mult_a;
	s << d->mult_b;

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 8; i++) {
			s << d->chr_page[j][i];
		}
	}
	for (int i = 0; i < 8; i++)
		s << d->cpu_bank_wren[i];
	return true;
}

bool CpuMapper5::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	s >> d->prg_size;
	s >> d->chr_size;
	s >> d->sram_we_a;
	s >> d->sram_we_b;
	s >> d->graphic_mode;
	s >> d->nametable_mode;
	s >> d->nametable_type[0];
	s >> d->nametable_type[1];
	s >> d->nametable_type[2];
	s >> d->nametable_type[3];
	s >> d->sram_page;
	s >> d->fill_chr;
	s >> d->fill_pal;
	s >> d->split_control;
	s >> d->split_scroll;
	s >> d->split_page;
	s >> d->chr_mode;
	s >> d->irq_status;
	s >> d->irq_enable;
	s >> d->irq_line;
	s >> d->irq_scanline;
	s >> d->irq_clear;
	s >> d->mult_a;
	s >> d->mult_b;

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 8; i++) {
			s >> d->chr_page[j][i];
		}
	}
	for (int i = 0; i < 8; i++)
		s >> d->cpu_bank_wren[i];

	ppuMapper->updateBanks();
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(5, "Nintendo MMC5")
