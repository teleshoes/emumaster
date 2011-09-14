#ifndef IMACHINE_H
#define IMACHINE_H

#include "base_global.h"
#include <QObject>
#include <QRectF>
class QImage;
class QSettings;
class QProcess;

typedef qint8 s8;
typedef quint8 u8;
typedef qint16 s16;
typedef quint16 u16;
typedef qint32 s32;
typedef quint32 u32;
typedef qint64 s64;
typedef quint64 u64;

class BASE_EXPORT IMachine : public QObject {
	Q_OBJECT
public:
	enum PadKey {
		Left_PadKey		= (1 <<  0),
		Right_PadKey	= (1 <<  1),
		Up_PadKey		= (1 <<  2),
		Down_PadKey		= (1 <<  3),
		A_PadKey		= (1 <<  4),
		B_PadKey		= (1 <<  5),
		X_PadKey		= (1 <<  6),
		Y_PadKey		= (1 <<  7),
		L_PadKey		= (1 <<  8),
		R_PadKey		= (1 <<  9),
		L2_PadKey		= (1 <<  8),
		R2_PadKey		= (1 <<  9),
		Start_PadKey	= (1 << 10),
		Select_PadKey	= (1 << 11)
	};
	static QString installationDirPath();
	static QString userDataDirPath();
	static QString diskDirPath(const QString &machineName);

	static void buildLocalDirTree();

	explicit IMachine(const QString &name, QObject *parent = 0);
	~IMachine();
	virtual QString init() = 0;
	Q_INVOKABLE virtual void reset();

	QString name() const;
	QString diskDirPath() const;
	QString screenShotPath(const QString &diskName) const;

	qreal frameRate() const;
	QRectF videoSrcRect() const;

	virtual QString setDisk(const QString &path) = 0;
	virtual void emulateFrame(bool drawEnabled) = 0;
	virtual const QImage &frame() const = 0;
	virtual int fillAudioBuffer(char *stream, int streamSize) = 0;
	virtual void setPadKeys(int pad, int keys) = 0;

	virtual bool save(QDataStream &s) = 0;
	virtual bool load(QDataStream &s) = 0;

	virtual void saveSettings(QSettings &s);
	virtual void loadSettings(QSettings &s);
signals:
	void videoSrcRectChanged();
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

#define STATE_SERIALIZE_BEGIN_SAVE(t_,version_) \
	bool t_::save(QDataStream &s) { \
		int version__ = version_; \
		s << QString(#t_); \
		s << version__;

#define STATE_SERIALIZE_BEGIN_LOAD(t_,version_) \
	bool t_::load(QDataStream &s) { \
		QString ts_; \
		s >> ts_; \
		if (ts_ != #t_) \
			return false; \
		int version__; \
		s >> version__; \
		if (version__ > version_) \
			return false;

#define STATE_SERIALIZE_VERSION version__

#define STATE_SERIALIZE_END_SAVE(t) return true; }
#define STATE_SERIALIZE_END_LOAD(t) return true; }

#define STATE_SERIALIZE_PARENT_SAVE(t) if (!t::save(s)) return false;
#define STATE_SERIALIZE_PARENT_LOAD(t) if (!t::load(s)) return false;

#define STATE_SERIALIZE_VAR_SAVE(v) s << (v);
#define STATE_SERIALIZE_VAR_LOAD(v) s >> (v);
#define STATE_SERIALIZE_SUBCALL_SAVE(v) if (!(v).save(s)) return false;
#define STATE_SERIALIZE_SUBCALL_LOAD(v) if (!(v).load(s)) return false;
#define STATE_SERIALIZE_SUBCALL_PTR_SAVE(v) if (!(v)->save(s)) return false;
#define STATE_SERIALIZE_SUBCALL_PTR_LOAD(v) if (!(v)->load(s)) return false;

#define STATE_SERIALIZE_TEST_TYPE_SAVE true
#define STATE_SERIALIZE_TEST_TYPE_LOAD false

#define STATE_SERIALIZE_ARRAY_SAVE(array,size) \
	if (s.writeRawData(reinterpret_cast<const char *>(array), (size)) != (size)) \
		return false;
#define STATE_SERIALIZE_ARRAY_LOAD(array,size) \
	if (s.readRawData(reinterpret_cast<char *>(array), (size)) != (size)) \
		return false;

#endif // IMACHINE_H
