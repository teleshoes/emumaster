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

#include "imachine.h"
#include "configuration.h"
#include <QSettings>
#include <QFile>
#include <QImage>
#include <QCoreApplication>

// TODO rename IMachine to Emu
// TODO rename MachineView to EmuView

EMSL emsl;

IMachine::IMachine(const QString &name, QObject *parent) :
	QObject(parent),
	m_name(name),
	m_frameRate(1),
	m_running(true)
{
}

IMachine::~IMachine()
{
}

void IMachine::reset()
{
}

void IMachine::setFrameRate(qreal rate)
{
	m_frameRate = rate;
}

void IMachine::setVideoSrcRect(const QRectF &rect)
{
	if (m_videoSrcRect != rect) {
		m_videoSrcRect = rect;
		emit videoSrcRectChanged();
	}
}

/*!
	WARNING: It is called before init()
	TODO call after init() maybe private and emit
*/
void IMachine::setAudioEnabled(bool on)
{
	Q_UNUSED(on)
}

void EMSL::varNotExist(const QString &name)
{
	error = QObject::tr("\"%1->%2\" not exists").arg(currGroup).arg(name);
}

void EMSL::ioError()
{
	error = QObject::tr("IO error");
}

bool IMachine::saveInternal(QDataStream *stream)
{
	QByteArray ba;
	ba.reserve(1024 * 1024 * 2);

	QDataStream baStream(&ba, QIODevice::WriteOnly);
	baStream.setByteOrder(QDataStream::LittleEndian);
	baStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

	emsl.stream = &baStream;
	emsl.currAddr.clear();
	emsl.currGroup.clear();
	emsl.allAddr.clear();
	emsl.error.clear();

	emConf.sl();
	sl();

	bool succeded = emsl.error.isEmpty();
	if (succeded) {
		*stream << emsl.allAddr;
		*stream << ba;
	}
	return succeded;
}

bool IMachine::loadInternal(QDataStream *stream)
{
	QByteArray ba;
	*stream >> emsl.allAddr;
	*stream >> ba;

	QDataStream baStream(&ba, QIODevice::ReadOnly);
	baStream.setByteOrder(QDataStream::LittleEndian);
	baStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

	emsl.save = false;
	emsl.stream = &baStream;
	emsl.currGroup.clear();
	emsl.currAddr.clear();
	emsl.error.clear();

	emConf.sl();
	if (!emsl.loadConfOnly && emsl.error.isEmpty()) {
		emsl.abortIfLoadFails = true;
		sl();
	} else {
		emsl.abortIfLoadFails = false;
	}

	// "version" in conf could be replaced with old one, when loading old state
	// restore it to be current one
	emConf.setItem("version", QCoreApplication::applicationVersion());

	return emsl.error.isEmpty();
}

void EMSL::push()
{
	end();
	groupStack.append(currGroup);
}

void EMSL::pop()
{
	begin(groupStack.takeLast());
}

bool IMachine::saveState(const QString &statePath)
{
	emsl.save = true;

	QByteArray data;
	data.reserve(10*1024*1024);

	QDataStream s(&data, QIODevice::WriteOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s.setFloatingPointPrecision(QDataStream::SinglePrecision);
	if (!saveInternal(&s))
		return false;

	QFile file(statePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		emsl.error = tr("Could not open file for writing.");
		return false;
	}

	s.setDevice(&file);
	s << frame().copy(videoSrcRect().toRect());
	QByteArray compressed = qCompress(data);
	bool ok = (file.write(compressed) == compressed.size());
	file.close();
	if (!ok)
		file.remove();
	return ok;
}

bool IMachine::loadState(const QString &statePath)
{
	emsl.save = false;
	emsl.abortIfLoadFails = false;

	QFile file(statePath);
	if (!file.open(QIODevice::ReadOnly)) {
		emsl.error = tr("Could not open file.");
		return false;
	}

	QDataStream sOmit(&file);
	sOmit.setByteOrder(QDataStream::LittleEndian);
	sOmit.setFloatingPointPrecision(QDataStream::SinglePrecision);
	QImage omitFrame;
	sOmit >> omitFrame;

	QByteArray compressed = file.read(file.size() - file.pos());
	QByteArray data = qUncompress(compressed);
	file.close();
	compressed.clear();

	QDataStream s(&data, QIODevice::ReadOnly);
	s.setByteOrder(QDataStream::LittleEndian);
	s.setFloatingPointPrecision(QDataStream::SinglePrecision);

	return loadInternal(&s);
}

void IMachine::keybEnqueue(int *data, int key)
{
	// TODO declare num of pads and mouses as constants
	int *keyb = &data[(2+2)*4];
	int i = 0;
	for (; i < 4; i++) {
		if (!keyb[i])
			break;
	}
	if (i < 4)
		keyb[i] = key;
}

int IMachine::keybDequeue(int *data)
{
	int *keyb = &data[(2+2)*4];
	int key = keyb[0];
	keyb[0] = keyb[1];
	keyb[1] = keyb[2];
	keyb[2] = keyb[3];
	keyb[3] = 0;
	return key;
}

void IMachine::pause()
{
	m_running = false;
}

void IMachine::resume()
{
	m_running = true;
}

void EMSL::begin(const QString &groupName)
{
	currGroup = groupName;
	currAddr = allAddr.value(groupName);
}

void EMSL::end()
{
	if (save)
		allAddr[currGroup] = currAddr;
}

void EMSL::array(const QString &name, void *data, int size)
{
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
