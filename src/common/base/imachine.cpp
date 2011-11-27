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
#include <QSettings>

EMSL emsl;

IMachine::IMachine(const QString &name, QObject *parent) :
	QObject(parent),
	m_name(name),
	m_frameRate(1) {
}

IMachine::~IMachine() {
}

void IMachine::reset()
{}

void IMachine::setFrameRate(qreal rate)
{ m_frameRate = rate; }

void IMachine::setVideoSrcRect(const QRectF &rect) {
	if (m_videoSrcRect != rect) {
		m_videoSrcRect = rect;
		emit videoSrcRectChanged();
	}
}

void IMachine::saveSettings(QSettings &s)
{ Q_UNUSED(s) }
void IMachine::loadSettings(QSettings &s)
{ Q_UNUSED(s) }

void IMachine::setAudioEnabled(bool on)
{ Q_UNUSED(on) }
void IMachine::setAudioSampleRate(int sampleRate)
{ Q_UNUSED(sampleRate) }

void EMSL::varNotExist(const QString &name)
{ error = QObject::tr("\"%1\" not exist").arg(name); }

void EMSL::ioError()
{ error = QObject::tr("IO error"); }

bool IMachine::save(QDataStream *stream) {
	QByteArray ba;
	ba.reserve(1024 * 1024 * 2);
	QDataStream baStream(&ba, QIODevice::WriteOnly);

	emsl.save = true;
	emsl.stream = &baStream;
	emsl.currAddr.clear();
	emsl.currGroup.clear();
	emsl.allAddr.clear();
	emsl.error.clear();
	sl();
	bool succeded = emsl.error.isEmpty();
	if (succeded) {
		*stream << emsl.allAddr;
		*stream << ba;
	}
	return succeded;
}

bool IMachine::load(QDataStream *stream) {
	QByteArray ba;
	*stream >> emsl.allAddr;
	*stream >> ba;
	QDataStream baStream(&ba, QIODevice::ReadOnly);

	emsl.save = false;
	emsl.stream = &baStream;
	emsl.currGroup.clear();
	emsl.currAddr.clear();
	emsl.error.clear();
	sl();
	if (!emsl.error.isEmpty()) {
		qDebug("bad load %s", qPrintable(emsl.error));
	}
	return emsl.error.isEmpty();
}

void EMSL::push() {
	end();
	groupStack.append(currGroup);
}

void EMSL::pop() {
	begin(groupStack.takeLast());
}
