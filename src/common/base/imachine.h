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

#define S8_MIN SCHAR_MIN
#define S8_MAX SCHAR_MAX
#define U8_MAX UCHAR_MAX
#define S16_MIN SHRT_MIN
#define S16_MAX SHRT_MAX
#define U16_MAX USHRT_MAX
#define S32_MIN INT_MIN
#define S32_MAX INT_MAX
#define U32_MAX UINT_MAX

class BASE_EXPORT IMachine : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name CONSTANT)
public:
	enum PadKey {
		PadKey_Right	= (1 <<  0),
		PadKey_Down		= (1 <<  1),
		PadKey_Up		= (1 <<  2),
		PadKey_Left		= (1 <<  3),

		PadKey_A		= (1 <<  4),
		PadKey_B		= (1 <<  5),
		PadKey_X		= (1 <<  6),
		PadKey_Y		= (1 <<  7),

		PadKey_L1		= (1 <<  8),
		PadKey_R1		= (1 <<  9),
		PadKey_L2		= (1 <<  8),
		PadKey_R2		= (1 <<  9),

		PadKey_Start	= (1 << 10),
		PadKey_Select	= (1 << 11)
	};

	static int *padOffset(int *data, int pad);
	static const int *padOffset(const int *data, int pad);
	static int *mouseOffset(int *data, int mouse);
	static void keybEnqueue(int *data, int key);
	static int keybDequeue(int *data);

	explicit IMachine(const QString &name, QObject *parent = 0);
	~IMachine();
	QString name() const;

	qreal frameRate() const;
	QRectF videoSrcRect() const;

	virtual QString init(const QString &diskPath) = 0;
	virtual void shutdown() = 0;
	Q_INVOKABLE virtual void reset();

	virtual void emulateFrame(bool drawEnabled) = 0;
	virtual const QImage &frame() const = 0;
	virtual int fillAudioBuffer(char *stream, int streamSize) = 0;

	bool saveState(const QString &statePath);
	bool loadState(const QString &statePath);
signals:
	void videoSrcRectChanged();
protected:
	virtual void sl() = 0;
	void setFrameRate(qreal rate);
	void setVideoSrcRect(const QRectF &rect);

	virtual void setAudioEnabled(bool on);

	int m_inputData[8*4];
private:
	bool saveInternal(QDataStream *stream);
	bool loadInternal(QDataStream *stream);

	QString m_name;
	qreal m_frameRate;
	QRectF m_videoSrcRect;

	friend class MachineView;
	friend class HostInput;
};

inline QString IMachine::name() const
{ return m_name; }
inline qreal IMachine::frameRate() const
{ return m_frameRate; }
inline QRectF IMachine::videoSrcRect() const
{ return m_videoSrcRect; }

inline int *IMachine::padOffset(int *data, int pad)
{ Q_ASSERT(pad >= 0 && pad < 2); return &data[pad*4]; }
inline const int *IMachine::padOffset(const int *data, int pad)
{ Q_ASSERT(pad >= 0 && pad < 2); return &data[pad*4]; }
inline int *IMachine::mouseOffset(int *data, int mouse)
{ Q_ASSERT(mouse >= 0 && mouse < 2); return &data[(mouse+2)*4]; }

// emumaster save/load functionality

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

	bool abortIfLoadFails;
	bool loadConfOnly;

	QString error;
private:
	void varNotExist(const QString &name);
	void ioError();

	QList<QString> groupStack;
};

extern EMSL emsl;

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
		if (!stream->device()->seek(addr)) {
			ioError();
			return;
		}
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
