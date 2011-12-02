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

#include "touchinputdevice.h"
#include "hostvideo.h"
#include <QTouchEvent>
#include <QPainter>

TouchInputDevice::TouchInputDevice(QObject *parent) :
	HostInputDevice("touch", parent) {

	QStringList confList;
	confList << tr("None");
	confList << tr("Pad A");
	confList << tr("Pad B");
	confList << tr("Mouse A");
	confList << tr("Mouse B");
	setConfList(confList);

	QObject::connect(this, SIGNAL(confChanged()), SLOT(onConfChanged()));
}

void TouchInputDevice::processTouch(QEvent *e) {
	m_numPoints = 0;

	QTouchEvent *touchEvent = static_cast<QTouchEvent *>(e);
	QList<QTouchEvent::TouchPoint> points = touchEvent->touchPoints();

	for (int i = 0; i < points.size(); i++) {
		QTouchEvent::TouchPoint point = points.at(i);
		if (point.state() & Qt::TouchPointReleased)
			continue;

		m_points[m_numPoints] = point.pos().toPoint();
		m_numPoints++;
		if (m_numPoints >= MaxPoints)
			break;
	}

	m_converted = false;
}

void TouchInputDevice::onConfChanged() {
	m_converted = false;
	m_numPoints = 0;
	m_buttons = 0;
	m_mouseX = m_mouseY = 0;
}

void TouchInputDevice::update(int *data) {
	if (confIndex() <= 0)
		return;

	if (confIndex() <= 2) {
		if (!m_converted) {
			convertPad();
			m_converted = true;
		}
		int *pad = padOffset(data, confIndex()-1);
		*pad |= m_buttons;
	} else if (confIndex() <= 4) {
		if (!m_converted) {
			convertMouse();
			m_converted = true;
		}
		int *mouse = mouseOffset(data, confIndex()-3);
		mouse[0] = m_buttons;
		mouse[1] = m_mouseX;
		mouse[2] = m_mouseY;
	}
}

void TouchInputDevice::convertPad() {
	m_buttons = 0;

	for (int i = 0; i < m_numPoints; i++) {
		int x = m_points[i].x();
		int y = m_points[i].y();
		if (y >= HostVideo::Height-240) {
			y -= HostVideo::Height-240;
			if (x < 240) {
				m_buttons |= buttonsInCircle(x, y);
			} else if (x >= HostVideo::Width-240) {
				x -= HostVideo::Width-240;
				m_buttons |= buttonsInCircle(x, y) << 4;
			}
		}
		//TODO select,start,l1,r1,l2,r2
	}
}

void TouchInputDevice::convertMouse() {
	m_buttons = 0;
	m_mouseX = m_mouseY = 0;

	for (int i = 0; i < m_numPoints; i++) {
		int x = m_points[i].x();
		int y = m_points[i].y();
		if (y >= HostVideo::Height-240) {
			y -= HostVideo::Height-240;
			if (x < 240) {
				m_mouseX = x - 120;
				m_mouseY = y - 120;
			} else if (x >= HostVideo::Width-240) {
				x -= HostVideo::Width-240;
				m_buttons |= buttonsInCircle(x, y);
			}
		}
	}
}

int TouchInputDevice::buttonsInCircle(int x, int y) const {
	int buttons = 0;
	if (x < 60) {
		buttons |= Left_PadKey;
		if (y < 60)
			buttons |= Up_PadKey;
		else if (y >= 180)
			buttons |= Down_PadKey;
	} else if (x >= 180) {
		buttons |= Right_PadKey;
		if (y < 60)
			buttons |= Up_PadKey;
		else if (y >= 180)
			buttons |= Down_PadKey;
	} else {
		if (y < 60) {
			buttons |= Up_PadKey;
		} else if (y >= 180) {
			buttons |= Down_PadKey;
		} else {
			x -= 120;
			y -= 120;
			if (qAbs(x) > qAbs(y)) {
				if (x > 0)
					buttons |= Right_PadKey;
				else
					buttons |= Left_PadKey;
			} else {
				if (y > 0)
					buttons |= Down_PadKey;
				else
					buttons |= Up_PadKey;
			}
		}
	}
	return buttons;
}

void TouchInputDevice::paint(QPainter &painter, qreal opacity) {
	if (confIndex() <= 0)
		return;
	// TODO
//	painter->setOpacity(opacity);
}
