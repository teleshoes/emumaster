#include "mapper1.h"
#include <QDebug>

CpuMapper1::CpuMapper1(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
	reg[0] = 0x0C;		// D3=1,D2=1
	reg[1] = reg[2] = reg[3] = 0;
	shift = regbuf = 0;

	patch = 0;
	wram_patch = 0;

	if (romSize16KB() <= 16) {
		setRom16KBank(0, 0);
		setRom16KBank(1, romSize16KB() - 1);
	} else {
		// For 512K/1M byte Cartridge
		setRom16KBank(0, 0);
		setRom16KBank(1, 16 - 1);
		patch = 1;
	}
//	TODO if( VROM_8K_SIZE ) {
////		SetVROM_8K_Bank( 0 );
//	}

//	DWORD	crc = nes->rom->GetPROM_CRC();

//	if( crc == 0xb8e16bd0 ) {	// Snow Bros.(J)
//		patch = 2;
//	}
////	if( crc == 0x9b565541 ) {	// Triathron, The(J)
////		nes->SetFrameIRQmode( FALSE );
////	}
//	if( crc == 0xc96c6f04 ) {	// Venus Senki(J)
//		nes->SetRenderMethod( NES::POST_ALL_RENDER );
//	}
////	if( crc == 0x5e3f7004 ) {	// Softball Tengoku(J)
////	}

//	if( crc == 0x4d2edf70 ) {	// Night Rider(J)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//	}
//	if( crc == 0xcd2a73f0 ) {	// Pirates!(U)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//		patch = 2;
//	}

////	if( crc == 0x09efe54b ) {	// Majaventure - Mahjong Senki(J)
////		nes->SetFrameIRQmode( FALSE );
////	}

//	if( crc == 0x11469ce3 ) {	// Viva! Las Vegas(J)
//	}
//	if( crc == 0xd878ebf5 ) {	// Ninja Ryukenden(J)
//		nes->SetRenderMethod( NES::POST_ALL_RENDER );
//	}

////	if( crc == 0x7bd7b849 ) {	// Nekketsu Koukou - Dodgeball Bu(J)
////	}

//	if( crc == 0x466efdc2 ) {	// Final Fantasy(J)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//		nes->ppu->SetExtMonoMode( TRUE );
//	}
//	if( crc == 0xc9556b36 ) {	// Final Fantasy I&II(J)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//		nes->ppu->SetExtMonoMode( TRUE );
//		nes->SetSAVERAM_SIZE( 16*1024 );
//		wram_patch = 2;
//	}

//	if( crc == 0x717e1169 ) {	// Cosmic Wars(J)
//		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
//	}

//	if( crc == 0xC05D2034 ) {	// Snake's Revenge(U)
//		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
//	}

//	if( crc == 0xb8747abf		// Best Play - Pro Yakyuu Special(J)
//	 || crc == 0x29449ba9		// Nobunaga no Yabou - Zenkoku Ban(J)
//	 || crc == 0x2b11e0b0		// Nobunaga no Yabou - Zenkoku Ban(J)(alt)
//	 || crc == 0x4642dda6		// Nobunaga's Ambition(U)
//	 || crc == 0xfb69743a		// Aoki Ookami to Shiroki Mejika - Genghis Khan(J)
//	 || crc == 0x2225c20f		// Genghis Khan(U)
//	 || crc == 0xabbf7217		// Sangokushi(J)
//	) {

//		nes->SetSAVERAM_SIZE( 16*1024 );
//		wram_patch = 1;
//		wram_bank  = 0;
//		wram_count = 0;
//	}
}

void CpuMapper1::writeHigh(quint16 address, quint8 data) {
//	if (wram_patch == 1 && address == 0xBFFF) {
//		wram_count++;
//		wram_bank += data & 0x01;
//		if (wram_count == 5) {
//			if (wram_bank) {
//				SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
//			} else {
//				SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
//			}
//			wram_bank = wram_count = 0;
//		}
//	}
	if (patch != 1 ) {
		if ((address & 0x6000) != (last_addr & 0x6000)) {
			shift = regbuf = 0;
		}
		last_addr = address;
	}
	if (data & 0x80) {
		shift = regbuf = 0;
//		reg[0] = 0x0C;		// D3=1,D2=1
		reg[0] |= 0x0C;		// D3=1,D2=1
		return;
	}
	if (data & 0x01)
		regbuf |= 1 << shift;
	if (++shift < 5)
		return;
	address = (address >> 13) & 3;
	reg[address] = regbuf;

	regbuf = 0;
	shift = 0;
	if (patch != 1) {
		// For Normal Cartridge
		switch (address) {
		case 0:
			mapper()->ppuMemory()->setMirroring(mirroringFromRegs());
			break;
		case 1:
		case 2:
			qDebug() << "1,2 " << QString::number(reg[0], 16) << " " << QString::number(reg[address], 16);

			if (mapper()->ppuMemory()->romSize1KB()) {
				if (reg[0] & 0x10) {
					// CHR 4K bank lower($0000-$0FFF)
					mapper()->ppuMemory()->setRom4KBank(0, reg[1]);
					// CHR 4K bank higher($1000-$1FFF)
					mapper()->ppuMemory()->setRom4KBank(1, reg[2]);
				} else {
					// CHR 8K bank($0000-$1FFF)
					mapper()->ppuMemory()->setRomBank(reg[1] >> 1);
				}
			} else {
				// For Romancia
				if (reg[0] & 0x10)
					mapper()->ppuMemory()->setCram4KBank(0, reg[address]);
			}
			break;
		case 3:
			//qDebug() << "3 " << QString::number(reg[0], 16) << " " << QString::number(reg[3], 16);
			if (!(reg[0] & 0x08)) {
				// PRG 32K bank ($8000-$FFFF)
				setRomBank(reg[3] >> 1);
			} else {
				if (reg[0] & 0x04) {
					// PRG 16K bank ($8000-$BFFF)
					setRom16KBank(0, reg[3]);
					setRom16KBank(1, romSize16KB() - 1);
				} else {
					// PRG 16K bank ($C000-$FFFF)
					setRom16KBank(1, reg[3]);
					setRom16KBank(0, 0);
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
//		if( wram_patch == 2 ) {
//			if( !(reg[1] & 0x18) ) {
//				SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
//			} else {
//				SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
//			}
//		}
		if (address == 0)
			mapper()->ppuMemory()->setMirroring(mirroringFromRegs());
		// Register #1
		if (mapper()->ppuMemory()->romSize1KB()) {
			if (reg[0] & 0x10) {
				// CHR 4K bank lower($0000-$0FFF)
				mapper()->ppuMemory()->setRom4KBank(0, reg[1]);
			} else {
				// CHR 8K bank($0000-$1FFF)
				mapper()->ppuMemory()->setRomBank(reg[1] >> 1);
			}
		} else {
			// For Romancia
			if (reg[0] & 0x10)
				mapper()->ppuMemory()->setCram4KBank(0, reg[1]);
		}
		// Register #2
		if (mapper()->ppuMemory()->romSize1KB()) {
			if (reg[0] & 0x10) {
				// CHR 4K bank higher($1000-$1FFF)
				mapper()->ppuMemory()->setRom4KBank(1, reg[2]);
			}
		} else {
			// For Romancia
			if (reg[0] & 0x10)
				mapper()->ppuMemory()->setCram4KBank(1, reg[2]);
		}
		// Register #3
		if (!(reg[0] & 0x08)) {
			// PRG 32K bank ($8000-$FFFF)
			setRomBank((reg[3] & (0xF + promBase)) >> 1);
		} else {
			if (reg[0] & 0x04) {
				// PRG 16K bank ($8000-$BFFF)
				setRom16KBank(0, promBase + (reg[3] & 0x0F));
				if (romSize16KB() >= 32)
					setRom16KBank(1, promBase + 16 - 1);
			} else {
				// PRG 16K bank ($C000-$FFFF)
				setRom16KBank(1, promBase + (reg[3] & 0x0F));
				if (romSize16KB() >= 32)
					setRom16KBank(1, promBase);
			}
		}
	}
}

NesPpuMemoryMapper::Mirroring CpuMapper1::mirroringFromRegs() const {
	NesPpuMemoryMapper::Mirroring mirroring;
	switch (reg[0] & 3) {
	case 0: mirroring = NesPpuMemoryMapper::SingleLow; break;
	case 1: mirroring = NesPpuMemoryMapper::SingleHigh; break;
	case 2: mirroring = NesPpuMemoryMapper::Vertical; break;
	case 3: mirroring = NesPpuMemoryMapper::Horizontal; break;
	}
	return mirroring;
}

NES_MAPPER_PLUGIN_SOURCE(1, "Nintendo MMC1")
