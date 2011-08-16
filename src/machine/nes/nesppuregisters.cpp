#include "nesppuregisters.h"
#include "nesppu.h"
#include "nesppupalette.h"
#include "nesppumapper.h"
#include <QDataStream>

#define PPU() NesPpu *ppu = static_cast<NesPpu *>(parent())

NesPpuRegisters::NesPpuRegisters(NesPpu *ppu) :
	QObject(ppu) {
	qMemSet(m_regs, 0, sizeof(m_regs));
	m_toggle = false;
	m_dataLatch = 0;
	m_add = 1;
	m_bufferedData = 0;
	m_securityValue = 0x00;
}

void NesPpuRegisters::updateType() {
	PPU();
	switch (ppu->chipType()) {
	case NesPpu::PPU2C05_01:	m_securityValue = 0x1B; break;
	case NesPpu::PPU2C05_02:	m_securityValue = 0x3D; break;
	case NesPpu::PPU2C05_03:	m_securityValue = 0x1C; break;
	case NesPpu::PPU2C05_04:	m_securityValue = 0x1B; break;
	default:					m_securityValue = 0x00; break;
	}
}

void NesPpuRegisters::write(quint16 address, quint8 data) {
	Q_ASSERT(address < 8);
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
		ppu->m_refreshLatch |= (data & 3) << 10;
		/* the char ram bank points either 0x0000 or 0x1000 (page 0 or page 4) */
		ppu->m_tilePageOffset = (data & BackgroundTableCR0Bit) << 8;
		ppu->m_spritePageOffset = (data & SpriteTableCR0Bit) << 9;
		m_add = ((data & IncrementCR0Bit) ? 32 : 1);
		break;
	case Control1:
		m_regs[Control1] = data;
		ppu->m_palette->updateColorEmphasisAndMask();
		break;
	case SpriteRAMAddress:
		m_regs[SpriteRAMAddress] = data;
		break;
	case SpriteRAMIO:
		/* if the PPU is currently rendering the screen,
		 * 0xff is written instead of the desired data. */
		if (ppu->m_scanline < NesPpu::VisibleScreenHeight)
			data = 0xFF;
		ppu->m_spriteMemory[m_regs[SpriteRAMAddress]++] = data;
		break;
	case Scroll:
		if (m_toggle) {
			ppu->m_refreshLatch &= ~((0xF8 << 2) | (7 << 12));
			ppu->m_refreshLatch |= (data & 0xF8) << 2;
			ppu->m_refreshLatch |= (data & 7) << 12;
		} else {
			ppu->m_refreshLatch &= ~0x1F;
			ppu->m_refreshLatch |= (data >> 3) & 0x1F;
			ppu->m_scrollTileXOffset = data & 7;
			// TODO check if it works with loopy_shift = ppu->m_scrollTileXOffset if yes remove
		}
		m_toggle = !m_toggle;
		break;
	case VRAMAddress:
		if (m_toggle) {
			ppu->m_refreshLatch &= 0xFF00;
			ppu->m_refreshLatch |= data;
			ppu->m_vramAddress = ppu->m_refreshLatch;
			ppu->m_mapper->addressBusLatch(ppu->m_vramAddress);
		} else {
			ppu->m_refreshLatch &= 0x00FF;
			ppu->m_refreshLatch |= (data & 0x3F) << 8;
		}
		m_toggle = !m_toggle;
		break;
	case VRAMIO: {
		quint16 vramAddress = ppu->m_vramAddress & 0x3FFF;
		if (vramAddress >= NesPpu::PalettesAddress)
			ppu->m_palette->write(vramAddress & 0x1F, data);
		else if (ppu->m_mapper->bank1KType(vramAddress >> 10) != NesPpuMapper::VromBank)
			ppu->m_mapper->write(vramAddress, data);
		ppu->m_vramAddress += m_add;
		break;
	}
	default:
		break;
	}
	m_dataLatch = data;
}

quint8 NesPpuRegisters::read(quint16 address) {
	Q_ASSERT(address < 8);
	PPU();
	Register reg = static_cast<Register> (address);
	switch (reg) {
	case Status:
		/* the top 3 bits of the status register are the only ones that report data. The
		   remainder contain whatever was last in the PPU data latch, except on the RC2C05 (protection) */
		if (m_securityValue) {
			m_dataLatch = m_regs[Status] & (VBlankSRBit | Sprite0HitSRBit);
			m_dataLatch |= m_securityValue;
		} else {
			m_dataLatch = m_regs[Status] | (m_dataLatch & 0x1F);
		}
		/* reset hi/lo scroll toggle */
		m_toggle = false;
		/* if the vblank bit is set, clear all status bits but the 2 sprite flags */
		if (m_dataLatch & VBlankSRBit) {
			m_regs[Status] &= (Sprite0HitSRBit | SpriteMaxSRBit);
			ppu->updateVBlankOut();
		}
		break;
	case SpriteRAMIO:
		m_dataLatch = ppu->m_spriteMemory[m_regs[SpriteRAMAddress]];
		//	TODO increment SpriteRAMAdress ?? +virtuanes
		break;
	case VRAMIO: {
		quint16 vramAddress = ppu->m_vramAddress & 0x3FFF;
		ppu->m_vramAddress += m_add;
		if (vramAddress >= NesPpu::PalettesAddress)
			return ppu->m_palette->read(vramAddress & 0x1F);
		else
			m_dataLatch = m_bufferedData;
		m_bufferedData = ppu->m_mapper->read(vramAddress);
		break;
	}
	default:
		break;
	}
	return m_dataLatch;
}

void NesPpuRegisters::setVBlank(bool on) {
	PPU();
	if (on)
		m_regs[Status] |= VBlankSRBit;
	else
		m_regs[Status] &= ~(VBlankSRBit | Sprite0HitSRBit);
	ppu->updateVBlankOut();
}

bool NesPpuRegisters::save(QDataStream &s) {
	for (int i = 0; i < 4; i++)
		s << m_regs[i];
	s << m_toggle;
	s << m_dataLatch;
	s << m_add;
	s << m_bufferedData;
	s << m_securityValue;
	return true;
}

bool NesPpuRegisters::load(QDataStream &s) {
	for (int i = 0; i < 4; i++)
		s >> m_regs[i];
	s >> m_toggle;
	s >> m_dataLatch;
	s >> m_add;
	s >> m_bufferedData;
	s >> m_securityValue;
	return true;
}
