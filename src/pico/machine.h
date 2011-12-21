#ifndef MACHINE_H
#define MACHINE_H

#include <imachine.h>

class PicoMachine : public IMachine {
	Q_OBJECT
public:
	PicoMachine();
	QString init(const QString &diskPath);
	void shutdown();
	void reset();

	void emulateFrame(bool drawEnabled);
	const QImage &frame() const;
	int fillAudioBuffer(char *stream, int streamSize);
protected:
	void sl();
	void setAudioEnabled(bool on);
	void updateInput();
};

extern PicoMachine picoMachine;

#endif // MACHINE_H
