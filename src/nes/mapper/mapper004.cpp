/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "mapper004.h"
#include "ppu.h"
#include "disk.h"
#include <imachine.h>

void Mapper004::reset() {
	NesMapper::reset();

	qMemSet(reg, 0, sizeof(reg));
	prg0 = 0;
	prg1 = 1;
	updateCpuBanks();

	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0xFF;
	irq_request = 0;
	irq_preset = 0;
	irq_preset_vbl = 0;

	irq_type = IrqNone;

	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	updatePpuBanks();

	u32 crc = nesDiskCrc;

	if (crc == 0x5c707ac4) {	// Mother(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xcb106f49) {	// F-1 Sensation(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x14a01c70) {	// Gun-Dec(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xeffeea40) {	// For Klax(J)
		irq_type = IrqKlax;
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xc17ae2dc) {	// God Slayer - Haruka Tenkuu no Sonata(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x5a6860f1) {	// Shougi Meikan '92(J)
		irq_type = IrqShougimeikan;
	}
	if (crc == 0xae280e20) {	// Shougi Meikan '93(J)
		irq_type = IrqShougimeikan;
	}
	if (crc == 0xe19a2473) {	// Sugoro Quest - Dice no Senshi Tachi(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xa9a0d729) {	// Dai Kaijuu - Deburas(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xc5fea9f2) {	// Dai 2 Ji - Super Robot Taisen(J)
		irq_type = IrqDai2JiSuper;
	}
	if (crc == 0xd852c2f7) {	// Time Zone(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xecfd3c69) {	// Taito Chase H.Q.(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xaafe699c) {	// Ninja Ryukenden 3 - Yomi no Hakobune(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x6cc62c06) {	// Hoshi no Kirby - Yume no Izumi no Monogatari(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x8685f366) {	// Matendouji(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x8635fed1) {	// Mickey Mouse 3 - Yume Fuusen(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x7c7ab58e) {	// Walkuere no Bouken - Toki no Kagi Densetsu(J)
		nesPpu.setRenderMethod(NesPpu::PostRender);
	}
	if (crc == 0x26ff3ea2) {	// Yume Penguin Monogatari(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0x126ea4a0) {	// Summer Carnival '92 Recca(J)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}

	if (crc == 0x1d2e5018		// Rockman 3(J)
	 || crc == 0x6b999aaf) {	// Megaman 3(U)
		irq_type = IrqRockman3;
	}

	if (crc == 0xd88d48d7) {	// Kick Master(U)
		irq_type = IrqRockman3;
	}

	if (crc == 0xA67EA466) {	// Alien 3(U)
		nesPpu.setRenderMethod(NesPpu::TileRender);
	}

	if (crc == 0xe763891b) {	// DBZ2
		irq_type = IrqDBZ2;
	}

	// VS-Unisystem
	vs_patch = 0;
	vs_index = 0;

	if (crc == 0xeb2dba63		// VS TKO Boxing
	 || crc == 0x98cfe016) {	// VS TKO Boxing (Alt)
		vs_patch = 1;
	}
	if (crc == 0x135adf7c) {	// VS Atari RBI Baseball
		vs_patch = 2;
	}
	if (crc == 0xf9d3b0a3		// VS Super Xevious
	 || crc == 0x9924980a		// VS Super Xevious (b1)
	 || crc == 0x66bb838f) {	// VS Super Xevious (b2)
		vs_patch = 3;
	}
}

u8 Mapper004::readLow(u16 address) {
	if (!vs_patch) {
		if (address >= 0x5000 && address < 0x6000)
			return nesXram[address - 0x4000];
	} else {
		if (vs_patch == 1) {
			// VS TKO Boxing Security
			if (address == 0x5E00) {
				vs_index = 0;
				return 0x00;
			} else if (address == 0x5E01) {
				static u8 vsTKOBoxingSecurity[32] = {
					0xFF, 0xBF, 0xB7, 0x97, 0x97, 0x17, 0x57, 0x4F,
					0x6F, 0x6B, 0xEB, 0xA9, 0xB1, 0x90, 0x94, 0x14,
					0x56, 0x4E, 0x6F, 0x6B, 0xEB, 0xA9, 0xB1, 0x90,
					0xD4, 0x5C, 0x3E, 0x26, 0x87, 0x83, 0x13, 0x00
				};
				return vsTKOBoxingSecurity[(vs_index++) & 0x1F];
			}
		} else if (vs_patch == 2) {
			// VS Atari RBI Baseball Security
			if (address == 0x5E00) {
				vs_index = 0;
				return 0x00;
			} else if (address == 0x5E01) {
				if (vs_index++ == 9)
					return 0x6F;
				else
					return 0xB4;
			}
		} else if (vs_patch == 3) {
			// VS Super Xevious
			switch (address) {
			case 0x54FF:
				return 0x05;
			case 0x5678:
				if (vs_index)
					return 0x00;
				else
					return 0x01;
				break;
			case 0x578F:
				if (vs_index)
					return 0xD1;
				else
					return 0x89;
				break;
			case 0x5567:
				if (vs_index) {
					vs_index = 0;
					return 0x3E;
				} else {
					vs_index = 1;
					return 0x37;
				}
				break;
			default:
				break;
			}
		}
	}
	return NesMapper::readLow(address);
}

void Mapper004::writeLow(u16 address, u8 data) {
	if (address >= 0x5000 && address < 0x6000)
		nesXram[address - 0x4000] = data;
	else
		NesMapper::writeLow(address, data);
}

void Mapper004::writeHigh(u16 address, u8 data) {
	switch (address & 0xE001) {
	case 0x8000:
		reg[0] = data;
		updateCpuBanks();
		updatePpuBanks();
		break;
	case 0x8001:
		reg[1] = data;
		switch (reg[0] & 0x07) {
		case 0x00: chr01 = data & 0xFE; updatePpuBanks(); break;
		case 0x01: chr23 = data & 0xFE; updatePpuBanks(); break;
		case 0x02: chr4 = data; updatePpuBanks(); break;
		case 0x03: chr5 = data; updatePpuBanks(); break;
		case 0x04: chr6 = data; updatePpuBanks(); break;
		case 0x05: chr7 = data; updatePpuBanks(); break;
		case 0x06: prg0 = data; updateCpuBanks(); break;
		case 0x07: prg1 = data; updateCpuBanks(); break;
		}
		break;
	case 0xA000:
		reg[2] = data;
		if (nesMirroring != FourScreenMirroring) {
			if (data & 0x01)
				setMirroring(HorizontalMirroring);
			else
				setMirroring(VerticalMirroring);
		}
		break;
	case 0xA001:
		reg[3] = data;
		break;
	case 0xC000:
		reg[4] = data;
		if (irq_type == IrqKlax || irq_type == IrqRockman3)
			irq_counter = data;
		else
			irq_latch = data;
		if (irq_type == IrqDBZ2)
			irq_latch = 0x07;
		break;
	case 0xC001:
		reg[5] = data;
		if (irq_type == IrqKlax || irq_type == IrqRockman3) {
			irq_latch = data;
		} else {
			if ((nesPpuScanline < NesPpu::VisibleScreenHeight) || (irq_type == IrqShougimeikan)) {
				irq_counter |= 0x80;
				irq_preset = 0xFF;
			} else {
				irq_counter |= 0x80;
				irq_preset_vbl = 0xFF;
				irq_preset = 0;
			}
		}
		break;
	case 0xE000:
		reg[6] = data;
		irq_enable = 0;
		irq_request = 0;
		setIrqSignalOut(false);
		break;
	case 0xE001:
		reg[7] = data;
		irq_enable = 1;
		irq_request = 0;
		break;
	}
}

void Mapper004::updateCpuBanks() {
	if (reg[0] & 0x40)
		setRom8KBanks(nesRomSize8KB-2, prg1, prg0, nesRomSize8KB-1);
	else
		setRom8KBanks(prg0, prg1, nesRomSize8KB-2, nesRomSize8KB-1);
}

void Mapper004::horizontalSync() {
	if (irq_type == IrqKlax) {
		if (nesPpuScanline < NesPpu::VisibleScreenHeight && nesPpu.isDisplayOn()) {
			if (irq_enable) {
				if (irq_counter == 0) {
					irq_counter = irq_latch;
					irq_request = 0xFF;
				}
				if (irq_counter > 0)
					irq_counter--;
			}
		}
		if (irq_request)
			setIrqSignalOut(true);
	} else if (irq_type == IrqRockman3) {
		if (nesPpuScanline < NesPpu::VisibleScreenHeight && nesPpu.isDisplayOn()) {
			if (irq_enable) {
				if (!(--irq_counter)) {
					irq_request = 0xFF;
					irq_counter = irq_latch;
				}
			}
		}
		if (irq_request)
			setIrqSignalOut(true);
	} else {
		if (nesPpuScanline < NesPpu::VisibleScreenHeight && nesPpu.isDisplayOn()) {
			if (irq_preset_vbl) {
				irq_counter = irq_latch;
				irq_preset_vbl = 0;
			}
			if (irq_preset) {
				irq_counter = irq_latch;
				irq_preset = 0;
				if (irq_type == IrqDai2JiSuper && nesPpuScanline == 0) {
					irq_counter--;
				}
			} else if (irq_counter > 0) {
				irq_counter--;
			}
			if (irq_counter == 0) {
				if (irq_enable) {
					irq_request = 0xFF;
					setIrqSignalOut(true);
				}
				irq_preset = 0xFF;
			}
		}
	}
}

void Mapper004::updatePpuBanks() {
	if (nesVromSize1KB) {
		if (reg[0] & 0x80) {
			setVrom1KBank(4, chr01);
			setVrom1KBank(5, chr01+1);
			setVrom1KBank(6, chr23);
			setVrom1KBank(7, chr23+1);
			setVrom1KBank(0, chr4);
			setVrom1KBank(1, chr5);
			setVrom1KBank(2, chr6);
			setVrom1KBank(3, chr7);
		} else {
			setVrom1KBank(0, chr01);
			setVrom1KBank(1, chr01+1);
			setVrom1KBank(2, chr23);
			setVrom1KBank(3, chr23+1);
			setVrom1KBank(4, chr4);
			setVrom1KBank(5, chr5);
			setVrom1KBank(6, chr6);
			setVrom1KBank(7, chr7);
		}
	} else {
		if (reg[0] & 0x80) {
			setCram1KBank(4, (chr01+0)&0x07);
			setCram1KBank(5, (chr01+1)&0x07);
			setCram1KBank(6, (chr23+0)&0x07);
			setCram1KBank(7, (chr23+1)&0x07);
			setCram1KBank(0, chr4&0x07);
			setCram1KBank(1, chr5&0x07);
			setCram1KBank(2, chr6&0x07);
			setCram1KBank(3, chr7&0x07);
		} else {
			setCram1KBank(0, (chr01+0)&0x07);
			setCram1KBank(1, (chr01+1)&0x07);
			setCram1KBank(2, (chr23+0)&0x07);
			setCram1KBank(3, (chr23+1)&0x07);
			setCram1KBank(4, chr4&0x07);
			setCram1KBank(5, chr5&0x07);
			setCram1KBank(6, chr6&0x07);
			setCram1KBank(7, chr7&0x07);
		}
	}
}

void Mapper004::extSl() {
	emsl.array("reg", reg, sizeof(reg));
	emsl.var("prg0", prg0);
	emsl.var("prg1", prg1);
	emsl.var("chr01", chr01);
	emsl.var("chr23", chr23);
	emsl.var("chr4", chr4);
	emsl.var("chr5", chr5);
	emsl.var("chr6", chr6);
	emsl.var("chr7", chr7);
	emsl.var("irq_enable", irq_enable);
	emsl.var("irq_counter", irq_counter);
	emsl.var("irq_latch", irq_latch);
	emsl.var("irq_request", irq_request);
	emsl.var("irq_preset", irq_preset);
	emsl.var("irq_preset_vbl", irq_preset_vbl);
}
