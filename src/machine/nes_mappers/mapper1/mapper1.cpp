#include "mapper1.h"
#include <QDataStream>
#include <QDebug>

CpuMapper1::CpuMapper1(NesMapper *mapper) :
	NesCpuMemoryMapper(mapper) {
}

void CpuMapper1::reset() {
	NesCpuMemoryMapper::reset();
	m_ppuMemory = mapper()->ppuMemory();
	m_reg[0] = 0x0C; // D3=1,D2=1
	m_reg[1] = m_reg[2] = m_reg[3] = 0;
	m_shift = m_regbuf = 0;

	m_patch = 0;
	m_wramPatch = 0;
	m_wramBank = 0;
	m_wramCount = 0;

	if (romSize16KB() <= 16) {
		setRom16KBank(0, 0);
		setRom16KBank(1, romSize16KB()-1);
	} else {
		// For 512K/1M byte Cartridge
		setRom16KBank(0, 0);
		setRom16KBank(1, 16-1);
		m_patch = 1;
	}
//	TODO DWORD	crc = nes->rom->GetPROM_CRC();

//	if( crc == 0xb8e16bd0 ) {	// Snow Bros.(J)
//		patch = 2;
//	}
//	if( crc == 0xc96c6f04 ) {	// Venus Senki(J)
//		nes->SetRenderMethod( NES::POST_ALL_RENDER );
//	}
//	if( crc == 0x4d2edf70 ) {	// Night Rider(J)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//	}
//	if( crc == 0xcd2a73f0 ) {	// Pirates!(U)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//		patch = 2;
//	}
//	if( crc == 0x11469ce3 ) {	// Viva! Las Vegas(J)
//	}
//	if( crc == 0xd878ebf5 ) {	// Ninja Ryukenden(J)
//		nes->SetRenderMethod( NES::POST_ALL_RENDER );
//	}
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
	if (m_wramPatch == 1 && address == 0xBFFF) {
		m_wramCount++;
		m_wramBank += data & 0x01;
		if (m_wramCount == 5) {
//			TODO if (wram_bank)
//				SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
//			else
//				SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
			m_wramBank = m_wramCount = 0;
		}
	}
	if (m_patch != 1) {
		if ((address & 0x6000) != (m_lastAddress & 0x6000))
			m_shift = m_regbuf = 0;
		m_lastAddress = address;
	}
	if (data & 0x80) {
		m_shift = m_regbuf = 0;
		m_reg[0] |= 0x0C;		// D3=1,D2=1
		return;
	}
	if (data & 0x01)
		m_regbuf |= 1 << m_shift;
	if (++m_shift < 5)
		return;
	address = (address >> 13) & 3;
	m_reg[address] = m_regbuf;

	m_regbuf = 0;
	m_shift = 0;
	if (m_patch != 1) {
		// For Normal Cartridge
		switch (address) {
		case 0:
			m_ppuMemory->setMirroring(mirroringFromRegs());
			break;
		case 1:
		case 2:
			if (m_ppuMemory->romSize1KB()) {
				if (m_reg[0] & 0x10) {
					// CHR 4K bank lower($0000-$0FFF)
					m_ppuMemory->setRom4KBank(0, m_reg[1]);
					// CHR 4K bank higher($1000-$1FFF)
					m_ppuMemory->setRom4KBank(1, m_reg[2]);
				} else {
					// CHR 8K bank($0000-$1FFF)
					m_ppuMemory->setRomBank(m_reg[1] >> 1);
				}
			} else {
				// for Romancia
				if (m_reg[0] & 0x10)
					m_ppuMemory->setCram4KBank(0, m_reg[address]);
			}
			break;
		case 3:
			if (!(m_reg[0] & 0x08)) {
				// PRG 32K bank ($8000-$FFFF)
				setRomBank(m_reg[3] >> 1);
			} else {
				if (m_reg[0] & 0x04) {
					// PRG 16K bank ($8000-$BFFF)
					setRom16KBank(0, m_reg[3]);
					setRom16KBank(1, romSize16KB()-1);
				} else {
					// PRG 16K bank ($C000-$FFFF)
					setRom16KBank(1, m_reg[3]);
					setRom16KBank(0, 0);
				}
			}
			break;
		}
	} else {
		// For 512K/1M byte Cartridge
		int	promBase = 0;
		if (romSize16KB() >= 32)
			promBase = m_reg[1] & 0x10;
		// For FinalFantasy I&II
		if( m_wramPatch == 2 ) {
//			TODO if( !(reg[1] & 0x18) ) {
//				SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
//			} else {
//				SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
//			}
		}
		if (address == 0)
			m_ppuMemory->setMirroring(mirroringFromRegs());
		// Register #1 and #2
		if (m_ppuMemory->romSize1KB()) {
			if (m_reg[0] & 0x10) {
				// CHR 4K bank lower($0000-$0FFF)
				m_ppuMemory->setRom4KBank(0, m_reg[1]);
				// CHR 4K bank higher($1000-$1FFF)
				m_ppuMemory->setRom4KBank(1, m_reg[2]);
			} else {
				// CHR 8K bank($0000-$1FFF)
				m_ppuMemory->setRomBank(m_reg[1] >> 1);
			}
		} else {
			// For Romancia
			if (m_reg[0] & 0x10) {
				m_ppuMemory->setCram4KBank(0, m_reg[1]);
				m_ppuMemory->setCram4KBank(1, m_reg[2]);
			}
		}
		// Register #3
		if (!(m_reg[0] & 0x08)) {
			// PRG 32K bank ($8000-$FFFF)
			setRomBank((m_reg[3] & (0xF + promBase)) >> 1);
		} else {
			if (m_reg[0] & 0x04) {
				// PRG 16K bank ($8000-$BFFF)
				setRom16KBank(0, promBase + (m_reg[3] & 0x0F));
				if (romSize16KB() >= 32)
					setRom16KBank(1, promBase+16-1);
			} else {
				// PRG 16K bank ($C000-$FFFF)
				setRom16KBank(1, promBase + (m_reg[3] & 0x0F));
				if (romSize16KB() >= 32)
					setRom16KBank(1, promBase);
			}
		}
	}
}

NesPpuMemoryMapper::Mirroring CpuMapper1::mirroringFromRegs() const {
	switch (m_reg[0] & 3) {
	case 0: return NesPpuMemoryMapper::SingleLow;
	case 1: return NesPpuMemoryMapper::SingleHigh;
	case 2: return NesPpuMemoryMapper::Vertical;
	case 3: return NesPpuMemoryMapper::Horizontal;
	}
	Q_ASSERT(false);
	return NesPpuMemoryMapper::SingleLow;
}

void CpuMapper1::save(QDataStream &s) {
	NesCpuMemoryMapper::save(s);
	for (int i = 0; i < sizeof(m_reg); i++)
		s << m_reg[i];
	s << m_shift << m_regbuf;
	s << m_wramBank << m_wramCount;
}

bool CpuMapper1::load(QDataStream &s) {
	if (!NesCpuMemoryMapper::load(s))
		return false;
	for (int i = 0; i < sizeof(m_reg); i++)
		s >> m_reg[i];
	s >> m_shift >> m_regbuf;
	s >> m_wramBank >> m_wramCount;
	return true;
}

NES_MAPPER_PLUGIN_EXPORT(1, "Nintendo MMC1")
