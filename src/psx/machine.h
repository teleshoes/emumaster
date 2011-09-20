#ifndef PSXMACHINE_H
#define PSXMACHINE_H

#include <imachine.h>
#include <QThread>
#include <QSemaphore>

class PsxMachine : public IMachine {
	Q_OBJECT
public:	
	enum SystemType {
		NtscType = 0,
		PalType
	};
	enum CpuType {
		CpuDynarec = 0,
		CpuInterpreter
	};
	enum GpuType {
		GpuUnai = 0
	};
	enum SpuType {
		SpuNull = 0
	};

	explicit PsxMachine(QObject *parent = 0);
	QString init();
	void shutdown();
	void reset();
	void updateGpuScale(int w, int h);
	void flipScreen();

	QString setDisk(const QString &path);
	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	int fillAudioBuffer(char *stream, int streamSize);
	void setPadKeys(int pad, int keys);

	bool save(QDataStream &s);
	bool load(QDataStream &s);

	int systemType;
protected:
	void setAudioEnabled(bool on);
private:
	QSemaphore m_consSem;
	QSemaphore m_prodSem;
};

class PsxThread : public QThread {
	Q_OBJECT
public:
protected:
	void run();
};

extern PsxMachine psxMachine;

#endif // PSXMACHINE_H
