#include "registers.h"
#include "ppu.h"
#include "palette.h"

#define PPU() Nes2C0XPpu *ppu = static_cast<Nes2C0XPpu *>(parent())

Nes2C0XRegisters::Nes2C0XRegisters(Nes2C0XPpu *ppu) :
	QObject(ppu) {
	qMemSet(m_regs, 0, sizeof(m_regs));
	m_toggle = false;
	m_dataLatch = 0;
	m_add = 1;
	m_bufferedData = 0;
}

void Nes2C0XRegisters::updateType() {
	PPU();
	switch (ppu->chipType()) {
	case Nes2C0XPpu::PPU2C05_01: m_securityValue = 0x1B; break;
	case Nes2C0XPpu::PPU2C05_02: m_securityValue = 0x3D; break;
	case Nes2C0XPpu::PPU2C05_03: m_securityValue = 0x1C; break;
	case Nes2C0XPpu::PPU2C05_04: m_securityValue = 0x1B; break;
	default: m_securityValue = 0x00; break;
	}
}

void Nes2C0XRegisters::write(quint16 address, quint8 data) {
	PPU();
	if (m_securityValue && !(address & 6))
		address ^= 1;
	Register reg = static_cast<Register>(address);
	switch (reg) {
	case Control0:
		m_regs[Control0] = data;
		ppu->updateVBlankOut();
		/* update the name table number on our refresh latches */
		ppu->m_refreshLatch &= ~(3 << 10);
		ppu->m_refreshLatch |= static_cast<uint> (data & 3) << 10;
		/* the char ram bank points either 0x0000 or 0x1000 (page 0 or page 4) */
		ppu->m_tilePageOffset = ((data & BackgroundTableCR0Bit) ? 0x1000 : 0);
		ppu->m_spritePageOffset = ((data & SpriteTableCR0Bit) ? 0x1000 : 0);
		m_add = ((data & IncrementCR0Bit) ? 32 : 1);
		break;
	case Control1:
		m_regs[Control1] = data;
		ppu->m_palette->update();
		break;
	case SpriteRAMAddress:
		m_regs[SpriteRAMAddress] = data;
		break;
	case SpriteRAMIO:
		/* If the PPU is currently rendering the screen,
		 * 0xff is written instead of the desired data. */
		if (ppu->m_scanline < Nes2C0XPpu::VisibleScreenHeight)
			data = 0xFF;
		ppu->m_spriteMemory[m_regs[SpriteRAMAddress]++] = data;
		break;
	case Scroll:
		if (m_toggle) {
			ppu->m_refreshLatch &= ~((0x1F << 5) | (7 << 12));
			ppu->m_refreshLatch |= ((data >> 3) & 0x1F) << 5;
			ppu->m_refreshLatch |= (data & 7) << 12;
		} else {
			ppu->m_refreshLatch &= ~0x1F;
			ppu->m_refreshLatch |= (data >> 3) & 0x1F;
			ppu->m_scrollTileXOffset = data & 7;
		}
		m_toggle = !m_toggle;
		break;
	case VRAMAddress:
		if (m_toggle) {
			ppu->m_refreshLatch &= 0xFF00;
			ppu->m_refreshLatch |= data;
			ppu->m_vramAddress = ppu->m_refreshLatch;
			ppu->m_refreshData = ppu->m_refreshLatch;
		} else {
			ppu->m_refreshLatch &= 0x00FF;
			ppu->m_refreshLatch |= static_cast<uint> (data & 0x3F) << 8;
		}
		m_toggle = !m_toggle;
		break;
	case VRAMIO:
		ppu->write(ppu->m_vramAddress, data);
		ppu->m_vramAddress += m_add;
		break;
	default:
		break;
	}
	m_dataLatch = data;
}

quint8 Nes2C0XRegisters::read(quint16 address) {
	PPU();
	Register reg = static_cast<Register> (address);
	switch (reg) {
	case Status:
		/* The top 3 bits of the status register are the only ones that report data. The
		   remainder contain whatever was last in the PPU data latch, except on the RC2C05 (protection) */
		if (m_securityValue) {
			m_dataLatch = m_regs[Status] & (VBlankSRBit | Sprite0HitSRBit);
			m_dataLatch |= m_securityValue;
		} else {
			m_dataLatch = m_regs[Status] | (m_dataLatch & 0x1F);
		}
		/* Reset hi/lo scroll toggle */
		m_toggle = false;
		/* If the vblank bit is set, clear all status bits but the 2 sprite flags */
		if (m_dataLatch & VBlankSRBit) {
			m_regs[Status] &= (Sprite0HitSRBit | SpriteMaxSRBit);
			ppu->updateVBlankOut();
		}
		break;
	case SpriteRAMIO:
		m_dataLatch = ppu->m_spriteMemory[m_regs[SpriteRAMAddress]];
		//	TODO increment SpriteRAMAdress ??
		break;
	case VRAMIO:
		if ((ppu->m_vramAddress & Nes2C0XPpu::PalettesAddress) == Nes2C0XPpu::PalettesAddress) {
			m_dataLatch = ppu->read(ppu->m_vramAddress);
			/* buffer the mirrored NT data */
			m_bufferedData = ppu->read(ppu->m_vramAddress & 0x2FFF);
		} else {
			m_dataLatch = m_bufferedData;
			m_bufferedData = ppu->read(ppu->m_vramAddress);
		}
		ppu->m_vramAddress += m_add;
		break;
	default:
		break;
	}
	return m_dataLatch;
}

void Nes2C0XRegisters::setVBlank() {
	PPU();
	m_regs[Status] |= VBlankSRBit;
	/* If NMI's are set to be triggered, go for it */
	ppu->updateVBlankOut();
	// TODO We need an ever-so-slight delay between entering vblank and firing an NMI - enough so that
	// a game can read the high bit of $2002 before the NMI is called (potentially resetting the bit
	// via a read from $2002 in the NMI handler).
	// B-Wings is an example game that needs this.
}

void Nes2C0XRegisters::clearStatus() {
	PPU();
	m_regs[Status] &= ~(VBlankSRBit | Sprite0HitSRBit | SpriteMaxSRBit);
	ppu->updateVBlankOut();
}
