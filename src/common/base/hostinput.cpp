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

#include "hostinput.h"
#include "hostvideo.h"
#include "imachine.h"
#include "touchinputdevice.h"
#include "accelinputdevice.h"
#include "sixaxisinputdevice.h"
#include <sixaxis.h>
#include <QKeyEvent>
#include <QTouchEvent>

HostInput::HostInput(IMachine *machine) :
	m_machine(machine) {
	m_padOpacity = 0.45f;

	m_touchInputDevice = new TouchInputDevice(this);
	m_accelInputDevice = new AccelInputDevice(this);

	SixAxisDaemon *daemon = SixAxisDaemon::instance();
	QObject::connect(daemon, SIGNAL(newPad()), SLOT(onSixAxisDetected()));
	daemon->start();
}

HostInput::~HostInput() {
	SixAxisDaemon::instance()->stop();
}

bool HostInput::eventFilter(QObject *o, QEvent *e) {
	Q_UNUSED(o)
	if (e->type() == QEvent::KeyPress || e->type() == QKeyEvent::KeyRelease) {
		bool state = (e->type() == QEvent::KeyPress);
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if (!ke->isAutoRepeat())
			processKey(static_cast<Qt::Key>(ke->key()), state);
		ke->accept();
		return true;
	} else if (e->type() == QEvent::TouchBegin ||
			   e->type() == QEvent::TouchUpdate ||
			   e->type() == QEvent::TouchEnd) {
		processTouch(e);
		e->accept();
		return true;
	} else {
		return false;
	}
}

void HostInput::processTouch(QEvent *e) {
	QTouchEvent *touchEvent = static_cast<QTouchEvent *>(e);
	const QList<QTouchEvent::TouchPoint> &points = touchEvent->touchPoints();
	for (int i = 0; i < points.size(); i++) {
		const QTouchEvent::TouchPoint &point = points.at(i);
		if (point.state() & Qt::TouchPointReleased)
			continue;
		int x = point.pos().x();
		int y = point.pos().y();
		if (y < 60) {
			if (x < 60)
				emit pause();
			else if (x > HostVideo::Width-60)
				emit quit();
		}
	}
	m_touchInputDevice->processTouch(e);
}

void HostInput::onSixAxisDetected() {
	SixAxisDaemon *daemon = SixAxisDaemon::instance();
	if (daemon->hasNewPad()) {
		SixAxis *sixAxis = daemon->nextNewPad();
		SixAxisInputDevice *sixAxisDev = new SixAxisInputDevice(sixAxis, this);
		QObject::connect(sixAxisDev, SIGNAL(destroyed()), SLOT(onSixAxisDestroyed()));
		m_sixAxisInputDevices.append(sixAxisDev);
		emit devicesChanged();
	}
}

void HostInput::onSixAxisDestroyed() {
	m_sixAxisInputDevices.removeOne(static_cast<SixAxisInputDevice *>(sender()));
	emit devicesChanged();
}

void HostInput::setPadOpacity(qreal opacity) {
	m_padOpacity = opacity;
}

void HostInput::paint(QPainter &painter) {
	m_touchInputDevice->paint(painter, m_padOpacity);
}

QList<QObject *> HostInput::devices() const {
	QList<QObject *> list;
	list.append(m_touchInputDevice);
	list.append(m_accelInputDevice);
	for (int i = 0; i < m_sixAxisInputDevices.size(); i++)
		list.append(m_sixAxisInputDevices.at(i));
	return list;
}

void HostInput::update() {
	int *data = m_machine->m_inputData;
	m_touchInputDevice->update(data);
	m_accelInputDevice->update(data);
	for (int i = 0; i < m_sixAxisInputDevices.size(); i++)
		m_sixAxisInputDevices.at(i)->update(data);
}
