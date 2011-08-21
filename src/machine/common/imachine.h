#ifndef IMACHINE_H
#define IMACHINE_H

#include "machine_common_global.h"
#include <QObject>
class QImage;

class MACHINE_COMMON_EXPORT IMachine : public QObject {
    Q_OBJECT
public:
	enum PadKey {
		Left_PadKey = 1,
		Right_PadKey,
		Up_PadKey,
		Down_PadKey,
		A_PadKey,
		B_PadKey,
		C_PadKey,
		X_PadKey,
		Y_PadKey,
		Z_PadKey,
		Start_PadKey,
		Select_PadKey
	};

    explicit IMachine(QObject *parent = 0);
	~IMachine();

	qreal frameRate() const;

	bool isAudioEnabled() const;
	bool isAudioStereo() const;
	int audioSampleRate() const;

	virtual void emulateFrame(bool drawEnabled) = 0;
	virtual const QImage &frame() const = 0;
	virtual const char *grabAudioBuffer(int *size) = 0;
	virtual void setPadKey(PadKey key, bool state) = 0;
protected:
	virtual void updateSettings() = 0;
	void setFrameRate(qreal rate);
private:
	qreal m_frameRate;
	bool m_audioEnable;
	bool m_audioStereoEnable;
	int m_audioSampleRate;

	friend class MachineView;
};

inline qreal IMachine::frameRate() const
{ return m_frameRate; }

inline bool IMachine::isAudioEnabled() const
{ return m_audioEnable; }
inline bool IMachine::isAudioStereo() const
{ return m_audioStereoEnable; }
inline int IMachine::audioSampleRate() const
{ return m_audioSampleRate; }

#endif // IMACHINE_H
