#ifndef GBAMACHINE_H
#define GBAMACHINE_H

class GbaPad;
class GbaMachine;
#include "imachine.h"
#include <QImage>
#include <QThread>
#include <QSemaphore>

class GbaThread : public QThread {
	Q_OBJECT
protected:
	void run();
};

class GbaMachine : public IMachine {
	Q_OBJECT
public:
	explicit GbaMachine(QObject *parent = 0);
	~GbaMachine();

	QString setDisk(const QString &path);
	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	int fillAudioBuffer(char *stream, int streamSize);
	void setPadKey(PadKey key, bool state);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	volatile bool m_quit;
	QSemaphore m_prodSem;
	QSemaphore m_consSem;
protected:
	void setAudioEnabled(bool on);
	void setAudioSampleRate(int sampleRate);
private:
	void loadBios();

	QString m_biosError;
	GbaPad *m_pad;
	QImage m_frame;
};

#endif // GBAMACHINE_H
