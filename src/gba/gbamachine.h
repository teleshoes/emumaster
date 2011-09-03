#ifndef GBAMACHINE_H
#define GBAMACHINE_H

class GBAPad;
#include "imachine.h"
#include <QImage>

class GBAMachine : public IMachine {
	Q_OBJECT
public:
	explicit GBAMachine(QObject *parent = 0);

	QString setDisk(const QString &path);
	quint32 diskCrc() const;
	QRectF videoSrcRect() const;
	QRectF videoDstRect() const;
	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	const char *grabAudioBuffer(int *size);
	void setPadKey(PadKey key, bool state);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

public slots:
	void invalidateFrame();


protected:
	void updateSettings();
private:
	void loadBios();

	QString m_biosError;
	GBAPad *m_pad;
	QImage m_frame;

	// TODO manage to remove
	char m_soundBuffer[16384];
	quint32 m_cycles;
};

#endif // GBAMACHINE_H
