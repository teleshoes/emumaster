#include "nesppupalette.h"
#include "nesppu.h"
#include <qmath.h>
#include <QDataStream>
#include <QDebug>

NesPpuPalette::NesPpuPalette(NesPpu *ppu) :
	QObject(ppu) {
	m_spritePenLut = &m_penLut[16];
	fill();
	qMemCopy(m_memory, m_defaultMemory, sizeof(m_memory));
	updateColorEmphasisAndMask();
}

void NesPpuPalette::updateColorEmphasisAndMask() {
	NesPpu *ppu = static_cast<NesPpu *>(parent());
	int newEmphasis = ppu->m_registers->colorEmphasis();
	int newMask = (ppu->m_registers->isMonochromeModeSet() ? 0xF0 : 0xFF);
	if (newEmphasis != m_emphasis || newMask != m_mask) {
		m_emphasis = newEmphasis;
		m_mask = newMask;
		m_penLutNeedsRebuild = true;
	}
}

void NesPpuPalette::write(quint16 address, quint8 data) {
	Q_ASSERT(address < 32);
	data &= 0x3F;
	if (!(address & 0x03)) {
		if (!(address & 0x0F)) {
			if (m_memory[0] != data) {
				for (int i = 0; i < 32; i += 4)
					m_memory[i] = data;
				m_penLutNeedsRebuild = true;
			}
		}
	} else {
		if (m_memory[address] != data) {
			m_memory[address] = data;
			m_penLutNeedsRebuild = true;
		}
	}
}

QRgb *NesPpuPalette::currentPens() {
	if (m_penLutNeedsRebuild) {
		rebuildPenLUT();
		m_penLutNeedsRebuild = false;
	}
	return m_penLut;
}

quint8 NesPpuPalette::read(quint16 address) const {
	Q_ASSERT(address < 32);
	return m_memory[address] & m_mask;
}

void NesPpuPalette::rebuildPenLUT() {
	for (int i = 0; i < 32; i++)
		m_penLut[i] = m_pens[(m_memory[i] & m_mask) + m_emphasis];
}

bool NesPpuPalette::save(QDataStream &s) {
	if (s.writeRawData(reinterpret_cast<const char *>(m_memory), sizeof(m_memory)) != sizeof(m_memory))
		return false;
	s << m_mask;
	s << m_emphasis;
	return true;
}

bool NesPpuPalette::load(QDataStream &s) {
	if (s.readRawData(reinterpret_cast<char *>(m_memory), sizeof(m_memory)) != sizeof(m_memory))
		return false;
	s >> m_mask;
	s >> m_emphasis;
	m_penLutNeedsRebuild = true;
	return true;
}

void NesPpuPalette::fill() {
	/* This routine builds a palette using a transformation from */
	/* the YUV (Y, B-Y, R-Y) to the RGB color space */

	/* The NES has a 64 color palette                        */
	/* 16 colors, with 4 luminance levels for each color     */
	/* The 16 colors circle around the YUV color space,      */

	int entry = 0;
	qreal tint = 0.22;	/* adjust to taste */
	qreal hue = 287.0;

	qreal Kr = 0.2989;
	qreal Kb = 0.1145;
	qreal Ku = 2.029;
	qreal Kv = 1.140;

	static const qreal brightness[3][4] = {
		{ 0.50, 0.75, 1.0, 1.0 },
		{ 0.29, 0.45, 0.73, 0.9 },
		{ 0, 0.24, 0.47, 0.77 }
	};
	/* Loop through the emphasis modes (8 total) */
	for (int colorEmphasis = 0; colorEmphasis < 8; colorEmphasis++) {
		/* loop through the 4 intensities */
		for (int colorIntensity = 0; colorIntensity < 4; colorIntensity++) {
			/* loop through the 16 colors */
			for (int colorNum = 0; colorNum < 16; colorNum++) {
				qreal sat;
				qreal y, u, v;
				qreal rad;

				switch (colorNum) {
				case 0:
					sat = 0; rad = 0;
					y = brightness[0][colorIntensity];
					break;
				case 13:
					sat = 0; rad = 0;
					y = brightness[2][colorIntensity];
					break;
				case 14:
				case 15:
					sat = 0; rad = 0; y = 0;
					break;
				default:
					sat = tint;
					rad = M_PI * ((colorNum * 30 + hue) / 180.0);
					y = brightness[1][colorIntensity];
					break;
				}
				u = sat * qCos(rad);
				v = sat * qSin(rad);
				/* Transform to RGB */
				qreal R = (y + Kv * v) * 255.0;
				qreal G = (y - (Kb * Ku * u + Kr * Kv * v) / (1 - Kb - Kr)) * 255.0;
				qreal B = (y + Ku * u) * 255.0;
				/* Clipping, in case of saturation */
				R = qMax(0.0, qMin(R, 255.0));
				G = qMax(0.0, qMin(G, 255.0));
				B = qMax(0.0, qMin(B, 255.0));
				/* emphasis */
				R = ((colorEmphasis & 1) ? 255.0 : R);
				G = ((colorEmphasis & 2) ? 255.0 : G);
				B = ((colorEmphasis & 4) ? 255.0 : B);
				/* Round, and set the value */
				m_pens[entry++] = qRgb(qFloor(R + 0.5), qFloor(G + 0.5), qFloor(B + 0.5));
			}
		}
	}
}

const quint8 NesPpuPalette::m_defaultMemory[] = {
	0,	1,	2,	3,
	0,	5,	6,	7,
	0,	9,	10,	11,
	0,	13,	14,	15,
	0,	17,	18,	19,
	0,	21,	22,	23,
	0,	25,	26,	27,
	0,	29,	30,	31
};
