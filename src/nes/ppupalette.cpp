#include "ppu.h"
#include <qmath.h>
#include <QDataStream>

static const u8 defaultMem[] = {
	0,	1,	2,	3,
	0,	5,	6,	7,
	0,	9,	10,	11,
	0,	13,	14,	15,
	0,	17,	18,	19,
	0,	21,	22,	23,
	0,	25,	26,	27,
	0,	29,	30,	31
};

static u8 paletteMem[32];
static QRgb pens[512];
static QRgb penLut[32];
static u32 mask;
static u32 emphasis;
static bool penLutNeedsRebuild;

static void fillPens();

static void buildPenLUT() {
	for (int i = 0; i < 32; i++)
		penLut[i] = pens[(paletteMem[i] & mask) + emphasis];
}

void NesPpuPalette::init() {
	fillPens();
	qMemCopy(paletteMem, defaultMem, sizeof(paletteMem));
	updateColorEmphasisAndMask();
}

void NesPpuPalette::updateColorEmphasisAndMask() {
	u32 newEmphasis = nesPpuRegisters.colorEmphasis();
	u32 newMask = (nesPpuRegisters.isMonochromeModeSet() ? 0xF0 : 0xFF);
	if (newEmphasis != emphasis || newMask != mask) {
		emphasis = newEmphasis;
		mask = newMask;
		penLutNeedsRebuild = true;
	}
}

void NesPpuPalette::write(u16 address, u8 data) {
	Q_ASSERT(address < 32);
	data &= 0x3F;
	if (!(address & 0x03)) {
		if (!(address & 0x0F)) {
			if (paletteMem[0] != data) {
				for (int i = 0; i < 32; i += 4)
					paletteMem[i] = data;
				penLutNeedsRebuild = true;
			}
		}
	} else {
		if (paletteMem[address] != data) {
			paletteMem[address] = data;
			penLutNeedsRebuild = true;
		}
	}
}

QRgb *NesPpuPalette::currentPens() {
	if (penLutNeedsRebuild) {
		buildPenLUT();
		penLutNeedsRebuild = false;
	}
	return penLut;
}

u8 NesPpuPalette::read(u16 address) const {
	Q_ASSERT(address < 32);
	return paletteMem[address] & mask;
}

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(NesPpuPalette, 1) \
	STATE_SERIALIZE_ARRAY_##sl(paletteMem, sizeof(paletteMem)) \
	STATE_SERIALIZE_VAR_##sl(mask) \
	STATE_SERIALIZE_VAR_##sl(emphasis) \
	m_penLutNeedsRebuild = true; \
STATE_SERIALIZE_END_##sl(NesPpuPalette)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)

static void fillPens() {
	/* This routine builds a palette using a transformation from */
	/* the YUV (Y, B-Y, R-Y) to the RGB color space */

	/* The NES has a 64 color palette                        */
	/* 16 colors, with 4 luminance levels for each color     */
	/* The 16 colors circle around the YUV color space,      */

	int entry = 0;
	qreal tint = 0.22f;	/* adjust to taste */
	qreal hue = 287.0f;

	qreal Kr = 0.2989f;
	qreal Kb = 0.1145f;
	qreal Ku = 2.029f;
	qreal Kv = 1.140f;

	static const qreal brightness[3][4] = {
		{ 0.50f, 0.75f,  1.0f,  1.0f },
		{ 0.29f, 0.45f, 0.73f,  0.9f },
		{ 0.0f,  0.24f, 0.47f, 0.77f }
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
					sat = 0.0f; rad = 0.0f;
					y = brightness[0][colorIntensity];
					break;
				case 13:
					sat = 0.0f; rad = 0.0f;
					y = brightness[2][colorIntensity];
					break;
				case 14:
				case 15:
					sat = 0.0f; rad = 0.0f; y = 0.0f;
					break;
				default:
					sat = tint;
					rad = M_PI * (qreal(qreal(colorNum) * 30.0f + hue) / 180.0f);
					y = brightness[1][colorIntensity];
					break;
				}
				u = sat * qCos(rad);
				v = sat * qSin(rad);
				/* Transform to RGB */
				qreal R = (y + Kv * v) * 255.0f;
				qreal G = (y - (Kb * Ku * u + Kr * Kv * v) / (1 - Kb - Kr)) * 255.0f;
				qreal B = (y + Ku * u) * 255.0f;
				/* Clipping, in case of saturation */
				R = qMax(qreal(0.0f), qMin(R, qreal(255.0f)));
				G = qMax(qreal(0.0f), qMin(G, qreal(255.0f)));
				B = qMax(qreal(0.0f), qMin(B, qreal(255.0f)));
				/* emphasis */
				R = ((colorEmphasis & 1) ? 255.0f : R);
				G = ((colorEmphasis & 2) ? 255.0f : G);
				B = ((colorEmphasis & 4) ? 255.0f : B);
				/* Round, and set the value */
				pens[entry++] = qRgb(qFloor(R + 0.5f), qFloor(G + 0.5f), qFloor(B + 0.5f));
			}
		}
	}
}
