#include "mapper1.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

CpuMapper1::CpuMapper1(NesMapper *mapper) :
	NesCpuMapper(mapper) {
}

void CpuMapper1::reset() {
	ppuMapper = mapper()->ppuMapper();
	reg[0] = 0x0C; // D3=1,D2=1
	reg[1] = reg[2] = reg[3] = 0;
	shift = regbuf = 0;

	patch = 0;
	wram_patch = 0;
	wram_bank = 0;
	wram_count = 0;

	if (romSize16KB() < 32) {
		setRom8KBanks(0, 1, romSize8KB()-2, romSize8KB()-1);
	} else {
		// For 512K/1M byte Cartridge
		setRom16KBank(4, 0);
		setRom16KBank(6, 16-1);
		patch = 1;
	}
	quint32 crc = disk()->crc();

	if (crc == 0xb8e16bd0) {	// Snow Bros.(J)
		patch = 2;
	}
	if (crc == 0xc96c6f04) {	// Venus Senki(J)
		ppu()->setRenderMethod(NesPpu::PostAllRender);
	}
	if (crc == 0x4d2edf70) {	// Night Rider(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xcd2a73f0) {	// Pirates!(U)
		ppu()->setRenderMethod(NesPpu::TileRender);
		patch = 2;
	}
	if (crc == 0x11469ce3) {	// Viva! Las Vegas(J)
	}
	if (crc == 0xd878ebf5) {	// Ninja Ryukenden(J)
		ppu()->setRenderMethod(NesPpu::PostAllRender);
	}
	if (crc == 0x466efdc2) {	// Final Fantasy(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
	}
	if (crc == 0xc9556b36) {	// Final Fantasy I&II(J)
		ppu()->setRenderMethod(NesPpu::TileRender);
		wram_patch = 2;
	}

	if (crc == 0x717e1169) {	// Cosmic Wars(J)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
	}

	if (crc == 0xC05D2034) {	// Snake's Revenge(U)
		ppu()->setRenderMethod(NesPpu::PreAllRender);
	}

	if (crc == 0xb8747abf		// Best Play - Pro Yakyuu Special(J)
	 || crc == 0x29449ba9		// Nobunaga no Yabou - Zenkoku Ban(J)
	 || crc == 0x2b11e0b0		// Nobunaga no Yabou - Zenkoku Ban(J)(alt)
	 || crc == 0x4642dda6		// Nobunaga's Ambition(U)
	 || crc == 0xfb69743a		// Aoki Ookami to Shiroki Mejika - Genghis Khan(J)
	 || crc == 0x2225c20f		// Genghis Khan(U)
	 || crc == 0xabbf7217		// Sangokushi(J)
	) {
		wram_patch = 1;
		wram_bank  = 0;
		wram_count = 0;
	}
}

void CpuMapper1::writeHigh(quint16 address, quint8 data) {
	if (wram_patch == 1 && address == 0xBFFF) {
		wram_count++;
		wram_bank += data & 0x01;
		if (wram_count == 5) {
			setWram8KBank(3, wram_bank ? 1 : 0);
			wram_bank = wram_count = 0;
		}
	}
	if (patch != 1) {
		if ((address & 0x6000) != (last_addr & 0x6000))
			shift = regbuf = 0;
		last_addr = address;
	}
	if (data & 0x80) {
		shift = regbuf = 0;
		reg[0] |= 0x0C;		// D3=1,D2=1
		return;
	}
	if (data & 0x01)
		regbuf |= 1 << shift;
	if (++shift < 5)
		return;
	address = (address&0x7FFF)>>13;
	reg[address] = regbuf;

	regbuf = 0;
	shift = 0;
	if (patch != 1) {
		// For Normal Cartridge
		switch (address) {
		case 0:
			ppuMapper->setMirroring(mirroringFromRegs());
			break;
		case 1:
		case 2:
			if (ppuMapper->vromSize1KB()) {
				if (reg[0] & 0x10) {
					// CHR 4K bank lower($0000-$0FFF)
					ppuMapper->setVrom4KBank(0, reg[1]);
					// CHR 4K bank higher($1000-$1FFF)
					ppuMapper->setVrom4KBank(4, reg[2]);
				} else {
					// CHR 8K bank($0000-$1FFF)
					ppuMapper->setVrom8KBank(reg[1] >> 1);
				}
			} else {
				// for Romancia
				if (reg[0] & 0x10)
					ppuMapper->setCram4KBank(0, reg[address]);
			}
			break;
		case 3:
			if (!(reg[0] & 0x08)) {
				// PRG 32K bank ($8000-$FFFF)
				setRom32KBank(reg[3] >> 1);
			} else {
				if (reg[0] & 0x04) {
					// PRG 16K bank ($8000-$BFFF)
					setRom16KBank(4, reg[3]);
					setRom16KBank(6, romSize16KB()-1);
				} else {
					// PRG 16K bank ($C000-$FFFF)
					setRom16KBank(6, reg[3]);
					setRom16KBank(4, 0);
				}
			}
			break;
		}
	} else {
		// For 512K/1M byte Cartridge
		int	promBase = 0;
		if (romSize16KB() >= 32)
			promBase = reg[1] & 0x10;
		// For FinalFantasy I&II
		if (wram_patch == 2) {
			if (!(reg[1] & 0x18))
				setWram8KBank(3, 0);
			else
				setWram8KBank(3, 1);
		}
		if (address == 0)
			ppuMapper->setMirroring(mirroringFromRegs());
		// Register #1 and #2
		if (ppuMapper->vromSize1KB()) {
			if (reg[0] & 0x10) {
				// CHR 4K bank lower($0000-$0FFF)
				ppuMapper->setVrom4KBank(0, reg[1]);
				// CHR 4K bank higher($1000-$1FFF)
				ppuMapper->setVrom4KBank(4, reg[2]);
			} else {
				// CHR 8K bank($0000-$1FFF)
				ppuMapper->setVrom8KBank(reg[1] >> 1);
			}
		} else {
			// For Romancia
			if (reg[0] & 0x10) {
				ppuMapper->setCram4KBank(0, reg[1]);
				ppuMapper->setCram4KBank(4, reg[2]);
			}
		}
		// Register #3
		if (!(reg[0] & 0x08)) {
			// PRG 32K bank ($8000-$FFFF)
			setRom32KBank((reg[3] & (0xF + promBase)) >> 1);
		} else {
			if (reg[0] & 0x04) {
				// PRG 16K bank ($8000-$BFFF)
				setRom16KBank(4, promBase + (reg[3] & 0x0F));
				if (romSize16KB() >= 32)
					setRom16KBank(6, promBase+16-1);
			} else {
				// PRG 16K bank ($C000-$FFFF)
				setRom16KBank(6, promBase + (reg[3] & 0x0F));
				if (romSize16KB() >= 32)
					setRom16KBank(4, promBase);
			}
		}
	}
}

NesPpuMapper::Mirroring CpuMapper1::mirroringFromRegs() const {
	switch (reg[0] & 3) {
	case 0: return NesPpuMapper::SingleLow;
	case 1: return NesPpuMapper::SingleHigh;
	case 2: return NesPpuMapper::Vertical;
	case 3: return NesPpuMapper::Horizontal;
	}
	Q_ASSERT(false);
	return NesPpuMapper::SingleLow;
}

bool CpuMapper1::save(QDataStream &s) {
	if (!NesCpuMapper::save(s))
		return false;
	for (int i = 0; i < sizeof(reg); i++)
		s << reg[i];
	s << shift << regbuf;
	s << wram_bank << wram_count;
	s << last_addr;
	return true;
}

bool CpuMapper1::load(QDataStream &s) {
	if (!NesCpuMapper::load(s))
		return false;
	for (int i = 0; i < sizeof(reg); i++)
		s >> reg[i];
	s >> shift >> regbuf;
	s >> wram_bank >> wram_count;
	s >> last_addr;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(1, "Nintendo MMC1")
