/*
	Free for non-commercial use.
	For commercial use, separate licencing terms must be obtained.
	(c) Copyright 2011, elemental
*/

#ifndef MACHINE_H
#define MACHINE_H

#include <imachine.h>
class Mp3Player;

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
	Mp3Player *mp3Player() const;

	void pause();
	void resume();
protected:
	void sl();
	void setAudioEnabled(bool on);
	void updateInput();
private:
	bool findMcdBios(QString *biosFileName, QString *error);

	int m_lastVideoMode;
	Mp3Player *m_mp3Player;
};

inline Mp3Player *PicoMachine::mp3Player() const
{ return m_mp3Player; }

extern PicoMachine picoMachine;

#endif // MACHINE_H
