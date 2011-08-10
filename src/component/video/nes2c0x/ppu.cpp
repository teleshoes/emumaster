#include "ppu.h"
#include "palette.h"
#include "registers.h"

Nes2C0XPpu::Nes2C0XPpu(QObject *parent) :
	QObject(parent),
	m_frame(VisibleScreenWidth, VisibleScreenHeight, QImage::Format_RGB32) {
	m_registers = new Nes2C0XRegisters(this);
	m_palette = new Nes2C0XPalette(this);
	setChipType(PPU2C02);

	m_scanline = 0;
	m_vramAddress = 0;
	m_refreshData = 0;
	m_refreshLatch = 0;
	m_scrollTileXOffset = 0;
	m_tilePageOffset = 0;
	m_spritePageOffset = 0;
	m_vBlankOut = false;
}

void Nes2C0XPpu::setChipType(ChipType newType) {
	m_type = newType;
	if (m_type == PPU2C07)
		m_scanlinesPerFrame = ScanlinesPerFramePAL;
	else
		m_scanlinesPerFrame = ScanlinesPerFrameNTSC;
	m_registers->updateType();
}

void Nes2C0XPpu::updateVBlankOut() {
	bool newVBlank = (m_registers->isVBlankEnabled() && m_registers->isVBlank());
	if (newVBlank != m_vBlankOut) {
		m_vBlankOut = newVBlank;
		emit vblank_o(m_vBlankOut);
	}
}
