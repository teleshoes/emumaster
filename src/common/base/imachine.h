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

	virtual void setAudioEnabled(bool on);
	virtual void setAudioSampleRate(int sampleRate);
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

#define STATE_SERIALIZE_BEGIN_SAVE(t) bool t::save(QDataStream &s) {
#define STATE_SERIALIZE_BEGIN_LOAD(t) bool t::load(QDataStream &s) {
#define STATE_SERIALIZE_END(t) return true; }

#define STATE_SERIALIZE_PARENT_SAVE(t) if (!t::save(s)) return false;
#define STATE_SERIALIZE_PARENT_LOAD(t) if (!t::load(s)) return false;

#define STATE_SERIALIZE_VAR_SAVE(v) s << (v);
#define STATE_SERIALIZE_VAR_LOAD(v) s >> (v);
#define STATE_SERIALIZE_SUBCALL_SAVE(v) if (!(v).save(s)) return false;
#define STATE_SERIALIZE_SUBCALL_LOAD(v) if (!(v).load(s)) return false;
#define STATE_SERIALIZE_SUBCALL_PTR_SAVE(v) if (!(v)->save(s)) return false;
#define STATE_SERIALIZE_SUBCALL_PTR_LOAD(v) if (!(v)->load(s)) return false;

#endif // IMACHINE_H
