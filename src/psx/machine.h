#ifndef PSXMACHINE_H
#define PSXMACHINE_H

#include <imachine.h>
#include <QThread>
#include <QSemaphore>

class PsxMachine : public IMachine {
	Q_OBJECT
public:
	explicit PsxMachine(QObject *parent = 0);
	void reset();
	void updateGpuScale(int w, int h);
	void flipScreen();

	QString setDisk(const QString &path);
	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	int fillAudioBuffer(char *stream, int streamSize);
	void setPadKey(PadKey key, bool state);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	QSemaphore m_consSem;
	QSemaphore m_prodSem;
	volatile bool m_quit;
};

class PsxThread : public QThread {
	Q_OBJECT
public:
protected:
	void run();
};

extern QImage gpuFrame;
extern PsxMachine psxMachine;

#endif // PSXMACHINE_H
