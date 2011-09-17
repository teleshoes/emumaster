#include "mapper005.h"
#include "disk.h"
#include "ppu.h"
#include <QDataStream>

void Mapper005::reset() {
	NesMapper::reset();

	cpu_bank_wren[0] = true;
	cpu_bank_wren[1] = true;
	cpu_bank_wren[2] = true;
	cpu_bank_wren[3] = true;
	cpu_bank_wren[4] = false;
	cpu_bank_wren[5] = false;
	cpu_bank_wren[6] = false;
	cpu_bank_wren[7] = false;

	prg_size = 3;
	chr_size = 3;

	sram_we_a = 0x00;
	sram_we_b = 0x00;

	graphic_mode = 0;
	nametable_mode = 0;

	for (uint i = 0; i < sizeof(nametable_type); i++)
		nametable_type[i] = 0;

	fill_chr = fill_pal = 0;
	split_control = split_scroll = split_page = 0;

	irq_enable = 0;
	irq_status = 0;
	irq_scanline = 0;
	irq_line = 0;
	irq_clear = 0;

	irq_type = IrqNone;

	mult_a = mult_b = 0;

	chr_type = 0;
	chr_mode = 0;

	for (int i = 0; i < 8; i++) {
		chr_page[0][i] = i;
		chr_page[1][i] = 4+(i&0x03);
	}
	for (int i = 4; i < 8; i++)
		setRom8KBank(i, nesRomSize8KB-1);
	setBankSram(3, 0);

	sram_size = 0;

	u32 crc = nesDiskCrc;

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
		sram_size = 1;
	} else
	if (crc == 0xf4120e58	// Aoki Ookami to Shiroki Mejika - Genchou Hishi(J)
	 || crc == 0x286613d8	// Nobunaga no Yabou - Bushou Fuuun Roku(J)
	 || crc == 0x11eaad26	// Romance of the Three Kingdoms 2(U)
	 || crc == 0x95ba5733) {	// Sangokushi 2(J)
		sram_size = 2;
	}

	if (crc == 0x95ca9ec7) { // Castlevania 3 - Dracula's Curse(U)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}

	if (crc == 0xcd9acf43) { // Metal Slader Glory(J)
		irq_type = IrqMetal;
	}

	if (crc == 0xe91548d8) { // Shin 4 Nin Uchi Mahjong - Yakuman Tengoku(J)
		chr_type = 1;
	}

	nesPpu.setExternalLatchEnabled(true);
// TODO ex sound	nes->apu->SelectExSound( 8);

	setVrom8KBank(0);

	for (int i = 0; i < 8; i++) {
		BG_MEM_BANK[i] = nesVrom+0x0400*i;
	}
}

u8 Mapper005::readLow(u16 address) {
	u8 data = address >> 8;

	switch (address) {
	case 0x5015:
		// TODO ex souund data = nes->apu->ExRead( address);
		break;

	case 0x5204:
		data = irq_status;
//		irq_status = 0;
		irq_status &= ~0x80;
		setIrqSignalOut(false);
		break;
	case 0x5205:
		data = mult_a*mult_b;
		break;
	case 0x5206:
		data = (u8)(((u16)mult_a*(u16)mult_b)>>8);
		break;
	}

	if (address >= 0x5C00 && address <= 0x5FFF) {
		if (graphic_mode >= 2) { // ExRAM mode
			data = nesVram[0x0800+(address&0x3FF)];
		}
	} else if (address >= 0x6000 && address <= 0x7FFF) {
		data = NesMapper::readLow(address);
	}
	return data;
}

void Mapper005::writeLow(u16 address, u8 data) {
	switch (address) {
	case 0x5100:
		prg_size = data & 0x03;
		break;
	case 0x5101:
		chr_size = data & 0x03;
		break;

	case 0x5102:
		sram_we_a = data & 0x03;
		break;
	case 0x5103:
		sram_we_b = data & 0x03;
		break;

	case 0x5104:
		graphic_mode = data & 0x03;
		break;
	case 0x5105:
		nametable_mode = data;
		for (int i = 0; i < 4; i++) {
			nametable_type[i] = data&0x03;
			setVram1KBank(8+i, nametable_type[i]);
			data >>= 2;
		}
		break;

	case 0x5106:
		fill_chr = data;
		break;
	case 0x5107:
		fill_pal = data & 0x03;
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
		chr_mode = 0;
		chr_page[0][address&0x07] = data;
		updatePpuBanks();
		break;

	case 0x5128:
	case 0x5129:
	case 0x512A:
	case 0x512B:
		chr_mode = 1;
		chr_page[1][(address&0x03)+0] = data;
		chr_page[1][(address&0x03)+4] = data;
		updatePpuBanks();
		break;

	case 0x5200:
		split_control = data;
		break;
	case 0x5201:
		split_scroll = data;
		break;
	case 0x5202:
		split_page = data&0x3F;
		break;

	case 0x5203:
		irq_line = data;
		setIrqSignalOut(false);
		break;
	case 0x5204:
		irq_enable = data;
		setIrqSignalOut(false);
		break;

	case 0x5205:
		mult_a = data;
		break;
	case 0x5206:
		mult_b = data;
		break;

	default:
		if (address >= 0x5000 && address <= 0x5015) {
			// TODO ex apu nes->apu->ExWrite( address, data);
		} else if (address >= 0x5C00 && address <= 0x5FFF) {
			if (graphic_mode == 2) {		// ExRAM
				nesVram[0x0800+(address&0x3FF)] = data;
			} else if (graphic_mode != 3) {	// Split,ExGraphic
				if (irq_status&0x40) {
					nesVram[0x0800+(address&0x3FF)] = data;
				} else {
					nesVram[0x0800+(address&0x3FF)] = 0;
				}
			}
		} else if (address >= 0x6000 && address <= 0x7FFF) {
			if ((sram_we_a == 0x02) && (sram_we_b == 0x01)) {
				if (cpu_bank_wren[3]) {
					writeDirect(address, data);
				}
			}
		}
		break;
	}
}

void Mapper005::writeHigh(u16 address, u8 data) {
	if (sram_we_a == 0x02 && sram_we_b == 0x01) {
		if (address >= 0x8000 && address < 0xE000) {
			if (cpu_bank_wren[address >> 13]) {
				writeDirect(address, data);
			}
		}
	}
}

void Mapper005::setBank(u16 address, u8 data) {
	if (data & 0x80) {
		// PROM Bank
		switch (address & 7) {
		case 4:
			if (prg_size == 3) {
				setRom8KBank(4, data&0x7F);
				cpu_bank_wren[4] = false;
			}
			break;
		case 5:
			if (prg_size == 1 || prg_size == 2) {
				setRom16KBank(4, (data&0x7F)>>1);
				cpu_bank_wren[4] = false;
				cpu_bank_wren[5] = false;
			} else if (prg_size == 3) {
				setRom8KBank(5, (data&0x7F));
				cpu_bank_wren[5] = false;
			}
			break;
		case 6:
			if (prg_size == 2 || prg_size == 3) {
				setRom8KBank(6, (data&0x7F));
				cpu_bank_wren[6] = false;
			}
			break;
		case 7:
			if (prg_size == 0) {
				setRom32KBank((data&0x7F)>>2);
				cpu_bank_wren[4] = false;
				cpu_bank_wren[5] = false;
				cpu_bank_wren[6] = false;
				cpu_bank_wren[7] = false;
			} else if (prg_size == 1) {
				setRom16KBank(6, (data&0x7F)>>1);
				cpu_bank_wren[6] = false;
				cpu_bank_wren[7] = false;
			} else if (prg_size == 2 || prg_size == 3) {
				setRom8KBank(7, (data&0x7F));
				cpu_bank_wren[7] = false;
			}
			break;
		}
	} else {
		// WRAM Bank
		switch (address & 7) {
		case 4:
			if (prg_size == 3) {
				setBankSram(4, data&0x07);
			}
			break;
		case 5:
			if (prg_size == 1 || prg_size == 2) {
				setBankSram(4, (data&0x06)+0);
				setBankSram(5, (data&0x06)+1);
			} else if (prg_size == 3) {
				setBankSram(5, data&0x07);
			}
			break;
		case 6:
			if (prg_size == 2 || prg_size == 3) {
				setBankSram(6, data&0x07);
			}
			break;
		}
	}
}

void Mapper005::setBankSram(u8 page, u8 data) {
	if (sram_size == 0) data = (data > 3) ? 8 : 0;
	if (sram_size == 1) data = (data > 3) ? 1 : 0;
	if (sram_size == 2) data = (data > 3) ? 8 : data;
	if (sram_size == 3) data = (data > 3) ? 4 : data;

	if (data != 8) {
		setWram8KBank(page, data);
		cpu_bank_wren[page] = true;
	} else {
		cpu_bank_wren[page] = false;
	}
}

void Mapper005::updatePpuBanks() {
	if (chr_mode == 0) {
		// PPU SP Bank
		switch (chr_size) {
		case 0:
			setVrom8KBank(chr_page[0][7]);
			break;
		case 1:
			setVrom4KBank(0, chr_page[0][3]);
			setVrom4KBank(4, chr_page[0][7]);
			break;
		case 2:
			setVrom2KBank(0, chr_page[0][1]);
			setVrom2KBank(2, chr_page[0][3]);
			setVrom2KBank(4, chr_page[0][5]);
			setVrom2KBank(6, chr_page[0][7]);
			break;
		case 3:
			for (int i = 0; i < 8; i++)
				setVrom1KBank(i, chr_page[0][i]);
			break;
		}
	} else if (chr_mode == 1) {
		// PPU BG Bank
		switch (chr_size) {
		case 0:
			for (int i = 0; i < 8; i++) {
				BG_MEM_BANK[i] = nesVrom+0x2000*(chr_page[1][7]%nesVromSize8KB)+0x0400*i;
			}
			break;
		case 1:
			for (int i = 0; i < 4; i++) {
				BG_MEM_BANK[i+0] = nesVrom+0x1000*(chr_page[1][3]%nesVromSize4KB)+0x0400*i;
				BG_MEM_BANK[i+4] = nesVrom+0x1000*(chr_page[1][7]%nesVromSize4KB)+0x0400*i;
			}
			break;
		case 2:
			for (int i = 0; i < 2; i++) {
				BG_MEM_BANK[i+0] = nesVrom+0x0800*(chr_page[1][1]%nesVromSize2KB)+0x0400*i;
				BG_MEM_BANK[i+2] = nesVrom+0x0800*(chr_page[1][3]%nesVromSize2KB)+0x0400*i;
				BG_MEM_BANK[i+4] = nesVrom+0x0800*(chr_page[1][5]%nesVromSize2KB)+0x0400*i;
				BG_MEM_BANK[i+6] = nesVrom+0x0800*(chr_page[1][7]%nesVromSize2KB)+0x0400*i;
			}
			break;
		case 3:
			for (int i = 0; i < 8; i++) {
				BG_MEM_BANK[i] = nesVrom+0x0400*(chr_page[1][i]%nesVromSize1KB);
			}
			break;
		}
	}
}

void Mapper005::horizontalSync(int scanline) {
	if (irq_type & IrqMetal) {
		if (irq_scanline == irq_line) {
			irq_status |= 0x80;
		}
	}
	if (nesPpu.isDisplayOn() && scanline < NesPpu::VisibleScreenHeight) {
		irq_scanline++;
		irq_status |= 0x40;
		irq_clear = 0;
	} else if (irq_type & IrqMetal) {
		irq_scanline = 0;
		irq_status &= ~0x80;
		irq_status &= ~0x40;
	}

	if (!(irq_type & IrqMetal)) {
		if (irq_scanline == irq_line) {
			irq_status |= 0x80;
		}

		if (++irq_clear > 2) {
			irq_scanline = 0;
			irq_status &= ~0x80;
			irq_status &= ~0x40;
			setIrqSignalOut(false);
		}
	}

	if ((irq_enable & 0x80) && (irq_status & 0x80) && (irq_status & 0x40)) {
		setIrqSignalOut(true);
	}

	// For Split mode!
	if (scanline == 0) {
		split_yofs = split_scroll&0x07;
		split_addr = ((split_scroll&0xF8)<<2);
	} else if (nesPpu.isDisplayOn()) {
		if (split_yofs == 7) {
			split_yofs = 0;
			if ((split_addr & 0x03E0) == 0x03A0) {
				split_addr &= 0x001F;
			} else {
				if ((split_addr & 0x03E0) == 0x03E0) {
					split_addr &= 0x001F;
				} else {
					split_addr += 0x0020;
				}
			}
		} else {
			split_yofs++;
		}
	}
}

void Mapper005::extensionLatchX(uint x) {
	split_x = x;
}

void Mapper005::extensionLatch(u16 address, u8 *plane1, u8 *plane2, u8 *attribute) {
	u16	ntbladr, attradr, tileadr;
	uint tilebank;
	bool bSplit;

	bSplit = false;
	if (split_control & 0x80) {
		if (!(split_control&0x40)) {
			// Left side
			if ((split_control&0x1F) > split_x) {
				bSplit = true;
			}
		} else {
			// Right side
			if ((split_control&0x1F) <= split_x) {
				bSplit = true;
			}
		}
	}

	if (!bSplit) {
		if (nametable_type[(address&0x0C00)>>10] == 3) {
			// Fill mode
			if (graphic_mode == 1) {
				// ExGraphic mode
				ntbladr = 0x2000+(address&0x0FFF);
				// Get Nametable
				tileadr = fill_chr*0x10+nesPpuScrollTileYOffset;
				// Get TileBank
				tilebank = 0x1000*((nesVram[0x0800+(ntbladr&0x03FF)]&0x3F)%nesVromSize4KB);
				// Attribute
				*attribute = (fill_pal<<2)&0x0C;
				// Get Pattern
				*plane1 = nesVrom[tilebank+tileadr  ];
				*plane2 = nesVrom[tilebank+tileadr+8];
			} else {
				// Normal
				tileadr = nesPpuTilePageOffset+fill_chr*0x10+nesPpuScrollTileYOffset;
				*attribute = (fill_pal<<2)&0x0C;
				// Get Pattern
				if (chr_type) {
					*plane1 = read(tileadr + 0);
					*plane2 = read(tileadr + 8);
				} else {
					*plane1 = BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
					*plane2 = BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
				}
			}
		} else if (graphic_mode == 1) {
			// ExGraphic mode
			ntbladr = 0x2000+(address&0x0FFF);
			// Get Nametable
			tileadr = (u16)read(ntbladr)*0x10+nesPpuScrollTileYOffset;
			// Get TileBank
			tilebank = 0x1000*((nesVram[0x0800+(ntbladr&0x03FF)]&0x3F)%nesVromSize4KB);
			// Get Attribute
			*attribute = (nesVram[0x0800+(ntbladr&0x03FF)]&0xC0)>>4;
			// Get Pattern
			*plane1 = nesVrom[tilebank+tileadr  ];
			*plane2 = nesVrom[tilebank+tileadr+8];
		} else {
			// Normal or ExVRAM
			ntbladr = 0x2000+(address&0x0FFF);
			attradr = 0x23C0+(address&0x0C00)+((address&0x0380)>>4)+((address&0x001C)>>2);
			// Get Nametable
			tileadr = nesPpuTilePageOffset+read(ntbladr)*0x10+nesPpuScrollTileYOffset;
			// Get Attribute
			*attribute = read(attradr);
			if (ntbladr & 0x0002) *attribute >>= 2;
			if (ntbladr & 0x0040) *attribute >>= 4;
			*attribute = (*attribute&0x03)<<2;
			// Get Pattern
			if (chr_type) {
				*plane1 = read(tileadr + 0);
				*plane2 = read(tileadr + 8);
			} else {
				*plane1 = BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
				*plane2 = BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			}
		}
	} else {
		ntbladr = ((split_addr&0x03E0)|(split_x&0x1F))&0x03FF;
		// Get Split TileBank
		tilebank = 0x1000*((int)split_page%nesVromSize4KB);
		tileadr  = (u16)nesVram[0x0800+ntbladr]*0x10+split_yofs;
		// Get Attribute
		attradr = 0x03C0+((ntbladr&0x0380)>>4)+((ntbladr&0x001C)>>2);
		*attribute = nesVram[0x0800+attradr];
		if (ntbladr & 0x0002) *attribute >>= 2;
		if (ntbladr & 0x0040) *attribute >>= 4;
		*attribute = (*attribute&0x03)<<2;
		// Get Pattern
		*plane1 = nesVrom[tilebank+tileadr  ];
		*plane2 = nesVrom[tilebank+tileadr+8];
	}
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(Mapper005, 1) \
	STATE_SERIALIZE_PARENT_##sl(NesMapper) \
	STATE_SERIALIZE_VAR_##sl(prg_size) \
	STATE_SERIALIZE_VAR_##sl(chr_size) \
	STATE_SERIALIZE_VAR_##sl(sram_we_a) \
	STATE_SERIALIZE_VAR_##sl(sram_we_b) \
	STATE_SERIALIZE_VAR_##sl(graphic_mode) \
	STATE_SERIALIZE_VAR_##sl(nametable_mode) \
	STATE_SERIALIZE_VAR_##sl(nametable_type[0]) \
	STATE_SERIALIZE_VAR_##sl(nametable_type[1]) \
	STATE_SERIALIZE_VAR_##sl(nametable_type[2]) \
	STATE_SERIALIZE_VAR_##sl(nametable_type[3]) \
	STATE_SERIALIZE_VAR_##sl(sram_page) \
	STATE_SERIALIZE_VAR_##sl(fill_chr) \
	STATE_SERIALIZE_VAR_##sl(fill_pal) \
	STATE_SERIALIZE_VAR_##sl(split_control) \
	STATE_SERIALIZE_VAR_##sl(split_scroll) \
	STATE_SERIALIZE_VAR_##sl(split_page) \
	STATE_SERIALIZE_VAR_##sl(chr_mode) \
	STATE_SERIALIZE_VAR_##sl(irq_status) \
	STATE_SERIALIZE_VAR_##sl(irq_enable) \
	STATE_SERIALIZE_VAR_##sl(irq_line) \
	STATE_SERIALIZE_VAR_##sl(irq_scanline) \
	STATE_SERIALIZE_VAR_##sl(irq_clear) \
	STATE_SERIALIZE_VAR_##sl(mult_a) \
	STATE_SERIALIZE_VAR_##sl(mult_b) \
	for (int j = 0; j < 2; j++) { \
		for (int i = 0; i < 8; i++) { \
			STATE_SERIALIZE_VAR_##sl(chr_page[j][i]) \
		} \
	} \
	for (int i = 0; i < 8; i++) \
		STATE_SERIALIZE_VAR_##sl(cpu_bank_wren[i]) \
	updatePpuBanks(); \
STATE_SERIALIZE_END_##sl(Mapper005)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
