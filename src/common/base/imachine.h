/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef IMACHINE_H
#define IMACHINE_H

#include "base_global.h"
#include <QObject>
#include <QHash>
#include <QRectF>
class QImage;
class QSettings;
class QProcess;
class Configuration;

typedef qint8 s8;
typedef quint8 u8;
typedef qint16 s16;
typedef quint16 u16;
typedef qint32 s32;
typedef quint32 u32;
typedef qint64 s64;
typedef quint64 u64;

// TODO consolide init with setDisk

class BASE_EXPORT IMachine : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name CONSTANT)
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

	explicit IMachine(const QString &name, QObject *parent = 0);
	~IMachine();
	QString name() const;

	Configuration *conf() const;

	qreal frameRate() const;
	QRectF videoSrcRect() const;

	virtual QString init(const QString &diskPath) = 0;
	virtual void shutdown() = 0;
	Q_INVOKABLE virtual void reset();

	virtual void emulateFrame(bool drawEnabled) = 0;
	virtual const QImage &frame() const = 0;
	virtual int fillAudioBuffer(char *stream, int streamSize) = 0;
	virtual void setPadKeys(int pad, int keys) = 0;

	bool save(QDataStream *stream);
	bool load(QDataStream *stream);
signals:
	void videoSrcRectChanged();
protected:
	virtual void sl() = 0;
	void setFrameRate(qreal rate);
	void setVideoSrcRect(const QRectF &rect);

	virtual void setAudioEnabled(bool on);
	virtual void setAudioSampleRate(int sampleRate);
private:
	QString m_name;
	qreal m_frameRate;
	QRectF m_videoSrcRect;
	Configuration *m_conf;

	friend class MachineView;
};

inline QString IMachine::name() const
{ return m_name; }
inline Configuration *IMachine::conf() const
{ return m_conf; }
inline qreal IMachine::frameRate() const
{ return m_frameRate; }
inline QRectF IMachine::videoSrcRect() const
{ return m_videoSrcRect; }

// EmuMaster Save/Load functionality

class EMSL {
public:
	void begin(const QString &groupName, int version);
	void end();

	void push();
	void pop();

	template <typename T>
	void var(const QString &name, T &t);

	void array(const QString &name, void *data, int size);

	QHash<QString, QHash<QString, int> > allAddr;

	QString currGroup;
	QHash<QString, int> currAddr;
	QDataStream *stream;
	bool save;
	int groupVersion;

	QString error;
private:
	void varNotExist(const QString &name);
	void ioError();

	QList<QString> groupStack;
};

extern EMSL emsl;

// TODO save/load begin/end checker

inline void EMSL::begin(const QString &groupName, int version = 1) {
	currGroup = groupName;
	currAddr = allAddr.value(groupName);
	var("v", version);
	groupVersion = version;
}

inline void EMSL::end() {
	if (save)
		allAddr[currGroup] = currAddr;
}

template <typename T>
inline void EMSL::var(const QString &name, T &t) {
	if (save) {
		currAddr.insert(name, stream->device()->pos());
		*stream << t;
	} else {
		int addr = currAddr.value(name, -1);
		if (addr < 0) {
			varNotExist(name);
			return;
		}
		stream->device()->seek(addr);
		*stream >> t;
	}
}

inline void EMSL::array(const QString &name, void *data, int size) {
	if (save) {
		currAddr.insert(name, stream->device()->pos());
		if (stream->writeRawData((const char *)data, size) != size) {
			ioError();
			return;
		}
	} else {
		int addr = currAddr.value(name, -1);
		if (addr < 0) {
			varNotExist(name);
			return;
		}
		stream->device()->seek(addr);
		if (stream->readRawData((char *)data, size) != size) {
			ioError();
			return;
		}
	}
}

#endif // IMACHINE_H
