#ifndef IMACHINE_H
#define IMACHINE_H

#include "base_global.h"
#include <QObject>
#include <QRectF>
class QImage;
class QSettings;
class QProcess;

class BASE_EXPORT IMachine : public QObject {
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
	static QString installationDirPath();
	static QString userDataDirPath();
	static QString diskDirPath(const QString &machineName);

	static void buildLocalDirTree();

	explicit IMachine(const QString &name, QObject *parent = 0);
	~IMachine();

	QString name() const;
	QString diskDirPath() const;
	QString screenShotPath(const QString &diskName) const;

	qreal frameRate() const;
	QRectF videoSrcRect() const;

	virtual QString setDisk(const QString &path) = 0;
	virtual void emulateFrame(bool drawEnabled) = 0;
	virtual const QImage &frame() const = 0;
	virtual int fillAudioBuffer(char *stream, int streamSize) = 0;
	virtual void setPadKey(PadKey key, bool state) = 0;

	virtual bool save(QDataStream &s) = 0;
	virtual bool load(QDataStream &s) = 0;

	virtual void saveSettings(QSettings &s);
	virtual void loadSettings(QSettings &s);
protected:
	void setFrameRate(qreal rate);
	void setVideoSrcRect(const QRectF &rect);

	virtual void setAudioEnabled(bool on) = 0;
	virtual void setAudioSampleRate(int sampleRate) = 0;
private:
	QString m_name;
	qreal m_frameRate;
	QRectF m_videoSrcRect;

	friend class MachineView;
};

inline QString IMachine::name() const
{ return m_name; }
inline QString IMachine::diskDirPath() const
{ return diskDirPath(name()); }
inline qreal IMachine::frameRate() const
{ return m_frameRate; }
inline QRectF IMachine::videoSrcRect() const
{ return m_videoSrcRect; }

#endif // IMACHINE_H
