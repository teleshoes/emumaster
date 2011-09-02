#ifndef IMACHINE_H
#define IMACHINE_H

class GameGenieCode;
#include "machine_common_global.h"
#include <QObject>
class QImage;
class QRectF;
class QSettings;

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
		X_PadKey,
		Y_PadKey,
		Start_PadKey,
		Select_PadKey,
		AllKeys
	};
	static IMachine *loadMachine(const QString &name);

	explicit IMachine(const QString &name, QObject *parent = 0);
	~IMachine();

	QString name() const;

	qreal frameRate() const;

	bool isAudioEnabled() const;
	bool isAudioStereo() const;
	int audioSampleRate() const;

	virtual QString setDisk(const QString &path) = 0;
	virtual quint32 diskCrc() const = 0;
	virtual QRectF videoSrcRect() const = 0;
	virtual QRectF videoDstRect() const = 0;
	virtual void emulateFrame(bool drawEnabled) = 0;
	virtual const QImage &frame() const = 0;
	// TODO pass buffer in not out
	virtual const char *grabAudioBuffer(int *size) = 0;
	virtual void setPadKey(PadKey key, bool state);

	virtual bool save(QDataStream &s) = 0;
	virtual bool load(QDataStream &s) = 0;

	virtual void saveSettings(QSettings &s);
	virtual void loadSettings(QSettings &s);

	// TODO move to nes
	virtual void setGameGenieCodeList(const QList<GameGenieCode> &codes);
protected:
	// TODO part this function
	virtual void updateSettings() = 0;
	void setFrameRate(qreal rate);
private:
	qreal m_frameRate;
	bool m_audioEnable;
	bool m_audioStereoEnable;
	int m_audioSampleRate;
	QString m_name;

	friend class MachineView;
};

inline QString IMachine::name() const
{ return m_name; }

inline qreal IMachine::frameRate() const
{ return m_frameRate; }

inline bool IMachine::isAudioEnabled() const
{ return m_audioEnable; }
inline bool IMachine::isAudioStereo() const
{ return m_audioStereoEnable; }
inline int IMachine::audioSampleRate() const
{ return m_audioSampleRate; }

#endif // IMACHINE_H
