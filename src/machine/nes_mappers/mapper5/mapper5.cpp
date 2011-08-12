#include "mapper5.h"
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
	quint8	BG_MEM_PAGE[8];

	quint8	irq_status;		// $5204(R)
	quint8	irq_enable;		// $5204(W)
	quint8	irq_line;		// $5203
	quint8	irq_scanline;
	quint8	irq_clear;		// HSync
	quint8	irq_type;

	quint8	mult_a, mult_b;		// $5205-$5206
};

CpuMapper5::CpuMapper5(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper),
	m_ppuMapper(0) {
	d = new NesMapper5Data();
}

CpuMapper5::~CpuMapper5() {
	delete d;
}

void CpuMapper5::reset() {
	NesCpuMemoryMapper::reset();
	m_ppuMapper = static_cast<PpuMapper5 *>(mapper()->ppuMemory());
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

	d->irq_type = 0;

	d->mult_a = d->mult_b = 0;

	d->chr_type = 0;
	d->chr_mode = 0;
	for (i = 0; i < 8; i++) {
		d->chr_page[0][i] = i;
		d->chr_page[1][i] = 4+(i&0x03);
	}
	for (int i = 0; i < 4; i++)
		setRom8KBank(i, romSize8KB()-1);
	m_ppuMapper->setRomBank(0);

	for (i = 0; i < 8; i++) {
		d->BG_MEM_BANK[i] = VROM+0x0400*i;
		d->BG_MEM_PAGE[i] = i;
	}
	SetBank_SRAM( 3, 0);

	d->sram_size = 0;
	nes->SetSAVERAM_SIZE( 16*1024);

/*	uint	crc = nes->rom->GetPROM_CRC();

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
		nes->SetSAVERAM_SIZE( 32*1024);
	} else
	if (crc == 0xf4120e58	// Aoki Ookami to Shiroki Mejika - Genchou Hishi(J)
	 || crc == 0x286613d8	// Nobunaga no Yabou - Bushou Fuuun Roku(J)
	 || crc == 0x11eaad26	// Romance of the Three Kingdoms 2(U)
	 || crc == 0x95ba5733) {	// Sangokushi 2(J)
		d->sram_size = 2;
		nes->SetSAVERAM_SIZE( 64*1024);
	}

	if (crc == 0x95ca9ec7) { // Castlevania 3 - Dracula's Curse(U)
		nes->SetRenderMethod( NES::TILE_RENDER);
	}

	if (crc == 0xcd9acf43) { // Metal Slader Glory(J)
		d->irq_type = IrqMetal;
	}

	if (crc == 0xe91548d8) { // Shin 4 Nin Uchi Mahjong - Yakuman Tengoku(J)
		d->chr_type = 1;
	}*/

	nes->ppu->SetExtLatchMode( TRUE);
	nes->apu->SelectExSound( 8);
}

quint8	Mapper005::ReadLow( quint16 addr)
{
quint8	data = (quint8)(addr>>8);

	switch (addr) {
	case 0x5015:
		data = nes->apu->ExRead( addr);
		break;

	case 0x5204:
		data = d->irq_status;
//		d->irq_status = 0;
		d->irq_status &= ~0x80;

		nes->cpu->ClrIRQ( IRQ_MAPPER);
		break;
	case 0x5205:
		data = d->mult_a*d->mult_b;
		break;
	case 0x5206:
		data = (quint8)(((quint16)d->mult_a*(quint16)d->mult_b)>>8);
		break;
	}

	if (addr >= 0x5C00 && addr <= 0x5FFF) {
		if (d->graphic_mode >= 2) { // ExRAM mode
			data = VRAM[0x0800+(addr&0x3FF)];
		}
	} else if (addr >= 0x6000 && addr <= 0x7FFF) {
		data = Mapper::ReadLow( addr);
	}

	return	data;
}

void	Mapper005::WriteLow( quint16 addr, quint8 data) {
	switch (addr) {
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
		for (i = 0; i < 4; i++) {
			d->nametable_type[i] = data&0x03;
			SetVRAM_1K_Bank( 8+i, d->nametable_type[i]);
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
		SetBank_SRAM( 3, data&0x07);
		break;

	case 0x5114:
	case 0x5115:
	case 0x5116:
	case 0x5117:
		SetBank_CPU( addr, data);
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
		d->chr_page[0][addr&0x07] = data;
		SetBank_PPU();
		break;

	case 0x5128:
	case 0x5129:
	case 0x512A:
	case 0x512B:
		d->chr_mode = 1;
		d->chr_page[1][(addr&0x03)+0] = data;
		d->chr_page[1][(addr&0x03)+4] = data;
		SetBank_PPU();
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

		nes->cpu->ClrIRQ( IRQ_MAPPER);
		break;
	case 0x5204:
		d->irq_enable = data;

		nes->cpu->ClrIRQ( IRQ_MAPPER);
		break;

	case 0x5205:
		d->mult_a = data;
		break;
	case 0x5206:
		d->mult_b = data;
		break;

	default:
		if (addr >= 0x5000 && addr <= 0x5015) {
			nes->apu->ExWrite( addr, data);
		} else if (addr >= 0x5C00 && addr <= 0x5FFF) {
			if (d->graphic_mode == 2) {		// ExRAM
				VRAM[0x0800+(addr&0x3FF)] = data;
			} else if (d->graphic_mode != 3) {	// Split,ExGraphic
				if (d->irq_status&0x40) {
					VRAM[0x0800+(addr&0x3FF)] = data;
				} else {
					VRAM[0x0800+(addr&0x3FF)] = 0;
				}
			}
		} else if (addr >= 0x6000 && addr <= 0x7FFF) {
			if ((d->sram_we_a == 0x02) && (d->sram_we_b == 0x01)) {
				if (CPU_MEM_TYPE[3] == BANKTYPE_RAM) {
					CPU_MEM_BANK[3][addr&0x1FFF] = data;
				}
			}
		}
		break;
	}
}

void	Mapper005::Write( quint16 addr, quint8 data)
{
	if (d->sram_we_a == 0x02 && d->sram_we_b == 0x01) {
		if (addr >= 0x8000 && addr < 0xE000) {
			if (CPU_MEM_TYPE[addr>>13] == BANKTYPE_RAM) {
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
			}
		}
	}
}

void	Mapper005::SetBank_CPU( quint16 addr, quint8 data)
{
	if (data & 0x80) {
	// PROM Bank
		switch (addr & 7) {
		case 4:
			if (d->prg_size == 3) {
				SetPROM_8K_Bank( 4, data&0x7F);
			}
			break;
		case 5:
			if (d->prg_size == 1 || d->prg_size == 2) {
				SetPROM_16K_Bank( 4, (data&0x7F)>>1);
			} else if (d->prg_size == 3) {
				SetPROM_8K_Bank( 5, (data&0x7F));
			}
			break;
		case 6:
			if (d->prg_size == 2 || d->prg_size == 3) {
				SetPROM_8K_Bank( 6, (data&0x7F));
			}
			break;
		case 7:
			if (d->prg_size == 0) {
				SetPROM_32K_Bank( (data&0x7F)>>2);
			} else if (d->prg_size == 1) {
				SetPROM_16K_Bank( 6, (data&0x7F)>>1);
			} else if (d->prg_size == 2 || d->prg_size == 3) {
				SetPROM_8K_Bank( 7, (data&0x7F));
			}
			break;
		}
	} else {
	// WRAM Bank
		switch (addr & 7) {
		case 4:
			if (d->prg_size == 3) {
				SetBank_SRAM( 4, data&0x07);
			}
			break;
		case 5:
			if (d->prg_size == 1 || d->prg_size == 2) {
				SetBank_SRAM( 4, (data&0x06)+0);
				SetBank_SRAM( 5, (data&0x06)+1);
			} else if (d->prg_size == 3) {
				SetBank_SRAM( 5, data&0x07);
			}
			break;
		case 6:
			if (d->prg_size == 2 || d->prg_size == 3) {
				SetBank_SRAM( 6, data&0x07);
			}
			break;
		}
	}
}

void	Mapper005::SetBank_SRAM( quint8 page, quint8 data)
{
	if (d->sram_size == 0) data = (data > 3) ? 8 : 0;
	if (d->sram_size == 1) data = (data > 3) ? 1 : 0;
	if (d->sram_size == 2) data = (data > 3) ? 8 : data;
	if (d->sram_size == 3) data = (data > 3) ? 4 : data;

	if (data != 8) {
		SetPROM_Bank( page, &WRAM[0x2000*data], BANKTYPE_RAM);
		CPU_MEM_PAGE[page] = data;
	} else {
		CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	}
}

void	Mapper005::SetBank_PPU()
{
INT	i;

	if (d->chr_mode == 0) {
	// PPU SP Bank
		switch (d->chr_size) {
		case 0:
			SetVROM_8K_Bank( d->chr_page[0][7]);
			break;
		case 1:
			SetVROM_4K_Bank( 0, d->chr_page[0][3]);
			SetVROM_4K_Bank( 4, d->chr_page[0][7]);
			break;
		case 2:
			SetVROM_2K_Bank( 0, d->chr_page[0][1]);
			SetVROM_2K_Bank( 2, d->chr_page[0][3]);
			SetVROM_2K_Bank( 4, d->chr_page[0][5]);
			SetVROM_2K_Bank( 6, d->chr_page[0][7]);
			break;
		case 3:
			SetVROM_8K_Bank( d->chr_page[0][0],
					 d->chr_page[0][1],
					 d->chr_page[0][2],
					 d->chr_page[0][3],
					 d->chr_page[0][4],
					 d->chr_page[0][5],
					 d->chr_page[0][6],
					 d->chr_page[0][7]);
			break;
		}
	} else if (d->chr_mode == 1) {
		// PPU BG Bank
		switch (d->chr_size) {
		case 0:
			for (i = 0; i < 8; i++) {
				d->BG_MEM_BANK[i] = VROM+0x2000*(d->chr_page[1][7]%VROM_8K_SIZE)+0x0400*i;
				d->BG_MEM_PAGE[i] = (d->chr_page[1][7]%VROM_8K_SIZE)*8+i;
			}
			break;
		case 1:
			for (i = 0; i < 4; i++) {
				d->BG_MEM_BANK[i+0] = VROM+0x1000*(d->chr_page[1][3]%VROM_4K_SIZE)+0x0400*i;
				d->BG_MEM_BANK[i+4] = VROM+0x1000*(d->chr_page[1][7]%VROM_4K_SIZE)+0x0400*i;
				d->BG_MEM_PAGE[i+0] = (d->chr_page[1][3]%VROM_4K_SIZE)*4+i;
				d->BG_MEM_PAGE[i+4] = (d->chr_page[1][7]%VROM_4K_SIZE)*4+i;
			}
			break;
		case 2:
			for (i = 0; i < 2; i++) {
				d->BG_MEM_BANK[i+0] = VROM+0x0800*(d->chr_page[1][1]%VROM_2K_SIZE)+0x0400*i;
				d->BG_MEM_BANK[i+2] = VROM+0x0800*(d->chr_page[1][3]%VROM_2K_SIZE)+0x0400*i;
				d->BG_MEM_BANK[i+4] = VROM+0x0800*(d->chr_page[1][5]%VROM_2K_SIZE)+0x0400*i;
				d->BG_MEM_BANK[i+6] = VROM+0x0800*(d->chr_page[1][7]%VROM_2K_SIZE)+0x0400*i;
				d->BG_MEM_PAGE[i+0] = (d->chr_page[1][1]%VROM_2K_SIZE)*2+i;
				d->BG_MEM_PAGE[i+2] = (d->chr_page[1][3]%VROM_2K_SIZE)*2+i;
				d->BG_MEM_PAGE[i+4] = (d->chr_page[1][5]%VROM_2K_SIZE)*2+i;
				d->BG_MEM_PAGE[i+6] = (d->chr_page[1][7]%VROM_2K_SIZE)*2+i;
			}
			break;
		case 3:
			for (i = 0; i < 8; i++) {
				d->BG_MEM_BANK[i] = VROM+0x0400*(d->chr_page[1][i]%VROM_1K_SIZE);
				d->BG_MEM_PAGE[i] = (d->chr_page[1][i]%VROM_1K_SIZE)+i;
			}
			break;
		}
	}
}

void	Mapper005::HSync( INT scanline)
{
	if (d->irq_type & IrqMetal) {
		if (d->irq_scanline == d->irq_line) {
			d->irq_status |= 0x80;
		}
	}
	if (nes->ppu->IsDispON() && scanline < 240) {
		d->irq_scanline++;
		d->irq_status |= 0x40;
		d->irq_clear = 0;
	} else if (d->irq_type & IrqMetal) {
		d->irq_scanline = 0;
		d->irq_status &= ~0x80;
		d->irq_status &= ~0x40;
	}

	if (!(d->irq_type & IrqMetal)) {
		if (d->irq_scanline == d->irq_line) {
			d->irq_status |= 0x80;
		}

		if (++d->irq_clear > 2) {
			d->irq_scanline = 0;
			d->irq_status &= ~0x80;
			d->irq_status &= ~0x40;

			nes->cpu->ClrIRQ( IRQ_MAPPER);
		}
	}

	if ((d->irq_enable & 0x80) && (d->irq_status & 0x80) && (d->irq_status & 0x40)) {
		nes->cpu->SetIRQ( IRQ_MAPPER);
///		nes->cpu->IRQ_NotPending();
	}

	// For Split mode!
	if (scanline == 0) {
		d->split_yofs = d->split_scroll&0x07;
		d->split_addr = ((d->split_scroll&0xF8)<<2);
	} else if (nes->ppu->IsDispON()) {
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

void	Mapper005::PPU_ExtLatchX( INT x)
{
	d->split_x = x;
}

void	Mapper005::PPU_ExtLatch( quint16 addr, quint8& chr_l, quint8& chr_h, quint8& attr)
{
quint16	ntbladr, attradr, tileadr, tileofs;
quint16	tile_yofs;
Dquint16	tilebank;
BOOL	bSplit;

	tile_yofs = nes->ppu->GetTILEY();

	bSplit = FALSE;
	if (d->split_control & 0x80) {
		if (!(d->split_control&0x40)) {
		// Left side
			if ((d->split_control&0x1F) > d->split_x) {
				bSplit = TRUE;
			}
		} else {
		// Right side
			if ((d->split_control&0x1F) <= d->split_x) {
				bSplit = TRUE;
			}
		}
	}

	if (!bSplit) {
		if (d->nametable_type[(addr&0x0C00)>>10] == 3) {
		// Fill mode
			if (d->graphic_mode == 1) {
			// ExGraphic mode
				ntbladr = 0x2000+(addr&0x0FFF);
				// Get Nametable
				tileadr = d->fill_chr*0x10+tile_yofs;
				// Get TileBank
				tilebank = 0x1000*((VRAM[0x0800+(ntbladr&0x03FF)]&0x3F)%VROM_4K_SIZE);
				// Attribute
				attr = (d->fill_pal<<2)&0x0C;
				// Get Pattern
				chr_l = VROM[tilebank+tileadr  ];
				chr_h = VROM[tilebank+tileadr+8];
			} else {
			// Normal
				tileofs = (PPUREG[0]&PPU_BGTBL_BIT)?0x1000:0x0000;
				tileadr = tileofs+d->fill_chr*0x10+tile_yofs;
				attr = (d->fill_pal<<2)&0x0C;
				// Get Pattern
				if (d->chr_type) {
					chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
					chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
				} else {
					chr_l = d->BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
					chr_h = d->BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
				}
			}
		} else if (d->graphic_mode == 1) {
		// ExGraphic mode
			ntbladr = 0x2000+(addr&0x0FFF);
			// Get Nametable
			tileadr = (quint16)PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+tile_yofs;
			// Get TileBank
			tilebank = 0x1000*((VRAM[0x0800+(ntbladr&0x03FF)]&0x3F)%VROM_4K_SIZE);
			// Get Attribute
			attr = (VRAM[0x0800+(ntbladr&0x03FF)]&0xC0)>>4;
			// Get Pattern
			chr_l = VROM[tilebank+tileadr  ];
			chr_h = VROM[tilebank+tileadr+8];
		} else {
		// Normal or ExVRAM
			tileofs = (PPUREG[0]&PPU_BGTBL_BIT)?0x1000:0x0000;
			ntbladr = 0x2000+(addr&0x0FFF);
			attradr = 0x23C0+(addr&0x0C00)+((addr&0x0380)>>4)+((addr&0x001C)>>2);
			// Get Nametable
			tileadr = tileofs+PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+tile_yofs;
			// Get Attribute
			attr = PPU_MEM_BANK[attradr>>10][attradr&0x03FF];
			if (ntbladr & 0x0002) attr >>= 2;
			if (ntbladr & 0x0040) attr >>= 4;
			attr = (attr&0x03)<<2;
			// Get Pattern
			if (d->chr_type) {
				chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
				chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			} else {
				chr_l = d->BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
				chr_h = d->BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			}
		}
	} else {
		ntbladr = ((d->split_addr&0x03E0)|(d->split_x&0x1F))&0x03FF;
		// Get Split TileBank
		tilebank = 0x1000*((INT)d->split_page%VROM_4K_SIZE);
		tileadr  = (quint16)VRAM[0x0800+ntbladr]*0x10+d->split_yofs;
		// Get Attribute
		attradr = 0x03C0+((ntbladr&0x0380)>>4)+((ntbladr&0x001C)>>2);
		attr = VRAM[0x0800+attradr];
		if (ntbladr & 0x0002) attr >>= 2;
		if (ntbladr & 0x0040) attr >>= 4;
		attr = (attr&0x03)<<2;
		// Get Pattern
		chr_l = VROM[tilebank+tileadr  ];
		chr_h = VROM[tilebank+tileadr+8];
	}
}

void	Mapper005::SaveState( LPquint8 p)
{
	p[ 0] = d->prg_size;
	p[ 1] = d->chr_size;
	p[ 2] = d->sram_we_a;
	p[ 3] = d->sram_we_b;
	p[ 4] = d->graphic_mode;
	p[ 5] = d->nametable_mode;
	p[ 6] = d->nametable_type[0];
	p[ 7] = d->nametable_type[1];
	p[ 8] = d->nametable_type[2];
	p[ 9] = d->nametable_type[3];
	p[10] = d->sram_page;
	p[11] = d->fill_chr;
	p[12] = d->fill_pal;
	p[13] = d->split_control;
	p[14] = d->split_scroll;
	p[15] = d->split_page;
	p[16] = d->chr_mode;
	p[17] = d->irq_status;
	p[18] = d->irq_enable;
	p[19] = d->irq_line;
	p[20] = d->irq_scanline;
	p[21] = d->irq_clear;
	p[22] = d->mult_a;
	p[23] = d->mult_b;

	INT	i, j;
	for (j = 0; j < 2; j++) {
		for (i = 0; i < 8; i++) {
			p[24+j*8+i] = d->chr_page[j][i];
		}
	}
//	for (i = 0; i < 8; i++) {
//		p[40+i] = d->BG_MEM_PAGE[i];
//	}
}

void	Mapper005::LoadState( LPquint8 p)
{
	d->prg_size	  = p[ 0];
	d->chr_size	  = p[ 1];
	d->sram_we_a	  = p[ 2];
	d->sram_we_b	  = p[ 3];
	d->graphic_mode	  = p[ 4];
	d->nametable_mode	  = p[ 5];
	d->nametable_type[0] = p[ 6];
	d->nametable_type[1] = p[ 7];
	d->nametable_type[2] = p[ 8];
	d->nametable_type[3] = p[ 9];
	d->sram_page	  = p[10];
	d->fill_chr	  = p[11];
	d->fill_pal	  = p[12];
	d->split_control	  = p[13];
	d->split_scroll	  = p[14];
	d->split_page	  = p[15];
	d->chr_mode          = p[16];
	d->irq_status	  = p[17];
	d->irq_enable	  = p[18];
	d->irq_line	  = p[19];
	d->irq_scanline	  = p[20];
	d->irq_clear	  = p[21];
	d->mult_a		  = p[22];
	d->mult_b		  = p[23];

	INT	i, j;

	for (j = 0; j < 2; j++) {
		for (i = 0; i < 8; i++) {
			d->chr_page[j][i] = p[24+j*8+i];
		}
	}
//	// BGƒoƒ“ƒN‚Ì�Ä�Ý’è�ˆ—�
//	for (i = 0; i < 8; i++) {
//		d->BG_MEM_PAGE[i] = p[40+i]%VROM_1K_SIZE;
//	}
//	for (i = 0; i < 8; i++) {
//		d->BG_MEM_BANK[i] = VROM+0x0400*d->BG_MEM_PAGE[i];
//	}

	SetBank_PPU();

}

NES_MAPPER_PLUGIN_SOURCE(5, "Nintendo MMC5")
