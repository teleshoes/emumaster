#ifndef NESPALETTE_H
#define NESPALETTE_H

#include "nes_global.h"
#include <QObject>
#include <QVector>
#include <QRgb>
class NesPpu;

class NES_EXPORT NesPpuPalette : public QObject {
	Q_OBJECT
public:
	explicit NesPpuPalette(NesPpu *ppu = 0);
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address) const;
	void updateColorEmphasisAndMask();

	QRgb *currentPens();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	void fill();
	void rebuildPenLUT();

	quint8 m_memory[32];
	QRgb m_pens[512];
	QRgb m_penLut[32];
	QRgb *m_spritePenLut;
	int m_mask;
	int m_emphasis;
	bool m_penLutNeedsRebuild;

	static const quint8 m_defaultMemory[];

	friend class NesPpu;
};

#endif // NESPALETTE_H
