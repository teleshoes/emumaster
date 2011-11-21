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
