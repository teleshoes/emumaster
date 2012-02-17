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

#ifndef EMU_H
#define EMU_H

#include "base_global.h"
#include <QObject>
#include <QHash>
#include <QRectF>
class QImage;

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

#define EM_MSG_DISK_LOAD_FAILED QObject::tr("Could not load the disk")
#define EM_MSG_STATE_DIFFERS QObject::tr("Configuration of loaded state differs from the current one. Mismatch in")

class BASE_EXPORT Emu : public QObject
{
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
		PadKey_L2		= (1 <<  10),
		PadKey_R2		= (1 <<  11),

		PadKey_Start	= (1 << 12),
		PadKey_Select	= (1 << 13)
	};

	static int *padOffset(int *data, int pad);
	static const int *padOffset(const int *data, int pad);
	static int *mouseOffset(int *data, int mouse);
	static void keybEnqueue(int *data, int key);
	static int keybDequeue(int *data);

	explicit Emu(const QString &name, QObject *parent = 0);
	~Emu();
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

	bool isRunning() const;
	virtual void pause();
	virtual void resume();
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
	bool m_running;

	friend class EmuView;
	friend class HostInput;
};

inline QString Emu::name() const
{ return m_name; }
inline qreal Emu::frameRate() const
{ return m_frameRate; }
inline QRectF Emu::videoSrcRect() const
{ return m_videoSrcRect; }

inline bool Emu::isRunning() const
{ return m_running; }

inline int *Emu::padOffset(int *data, int pad)
{ Q_ASSERT(pad >= 0 && pad < 2); return &data[pad*4]; }
inline const int *Emu::padOffset(const int *data, int pad)
{ Q_ASSERT(pad >= 0 && pad < 2); return &data[pad*4]; }
inline int *Emu::mouseOffset(int *data, int mouse)
{ Q_ASSERT(mouse >= 0 && mouse < 2); return &data[(mouse+2)*4]; }

// emumaster save/load functionality

class BASE_EXPORT EMSL
{
public:
	void begin(const QString &groupName);
	void end();

	void push();
	void pop();

	template <typename T> void var(const QString &name, T &t);
	void array(const QString &name, void *data, int size);

	QHash<QString, QHash<QString, int> > allAddr;

	QString currGroup;
	QHash<QString, int> currAddr;
	QDataStream *stream;
	bool save;

	bool abortIfLoadFails;
	bool loadConfOnly;

	QString error;
private:
	void varNotExist(const QString &name);
	void ioError();

	QList<QString> groupStack;
};

BASE_EXPORT extern EMSL emsl;

template <typename T>
inline void EMSL::var(const QString &name, T &t)
{
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

#endif // EMU_H
