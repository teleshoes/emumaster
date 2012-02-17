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
#include "pathmanager.h"
#include "emu.h"
#include <QTouchEvent>
#include <QPainter>

TouchInputDevice::TouchInputDevice(QObject *parent) :
	HostInputDevice("touch", QObject::tr("Touch Screen"), parent) {
	m_numPoints = 0;

	QStringList functionNameList;
	functionNameList << tr("None");
	functionNameList << tr("Pad A");
	functionNameList << tr("Pad B");
	functionNameList << tr("Mouse A");
	functionNameList << tr("Mouse B");
	setEmuFunctionNameList(functionNameList);

#if defined(MEEGO_EDITION_HARMATTAN)
	setEmuFunction(1);
#endif

	QObject::connect(this, SIGNAL(emuFunctionChanged()), SLOT(onEmuFunctionChanged()));

	m_padImage.load(pathManager.installationDirPath()+"/data/pad.png");
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

void TouchInputDevice::onEmuFunctionChanged() {
	m_converted = false;
	m_numPoints = 0;
	m_buttons = 0;
	m_mouseX = m_mouseY = 0;
}

void TouchInputDevice::update(int *data) {
	if (emuFunction() <= 0)
		return;

	if (emuFunction() <= 2) {
		if (!m_converted) {
			convertPad();
			m_converted = true;
		}
		int *pad = Emu::padOffset(data, emuFunction()-1);
		pad[0] |= m_buttons;
	} else if (emuFunction() <= 4) {
		if (!m_converted) {
			convertMouse();
			m_converted = true;
			int *mouse = Emu::mouseOffset(data, emuFunction()-3);
			mouse[0] = m_buttons >> 4;
			mouse[1] = m_mouseX - m_lastMouseX;
			mouse[2] = m_mouseY - m_lastMouseY;
		}
	}
}

void TouchInputDevice::convertPad() {
	m_buttons = 0;

	for (int i = 0; i < m_numPoints; i++) {
		int x = m_points[i].x();
		int y = m_points[i].y();
		if (y >= HostVideo::Height-CircleSize) {
			y -= HostVideo::Height-CircleSize;
			if (x < CircleSize) {
				// directions
				m_buttons |= buttonsInCircle(x, y);
			} else if (x >= HostVideo::Width-CircleSize) {
				// a,b,x,y
				x -= HostVideo::Width-CircleSize;
				m_buttons |= buttonsInCircle(x, y) << 4;
			} else if (x >= HostVideo::Width/2-ButtonWidth &&
					   x < HostVideo::Width/2+ButtonWidth) {
				// select, start
				if (y >= CircleSize-ButtonHeight) {
					if (x < HostVideo::Width/2)
						m_buttons |= Emu::PadKey_Select;
					else
						m_buttons |= Emu::PadKey_Start;
				}
			}
		} else if (y >= 120 && y < 120+ButtonHeight) {
			// l1,r1
			if (x < ButtonWidth)
				m_buttons |= Emu::PadKey_L1;
			else if (x >= HostVideo::Width-ButtonWidth)
				m_buttons |= Emu::PadKey_R1;
		}
	}
}

void TouchInputDevice::convertMouse() {
	m_buttons = 0;
	m_lastMouseX = m_mouseX;
	m_lastMouseY = m_mouseY;

	bool newMoving = false;
	for (int i = 0; i < m_numPoints; i++) {
		int x = m_points[i].x();
		int y = m_points[i].y();
		if (y >= HostVideo::Height-CircleSize) {
			y -= HostVideo::Height-CircleSize;
			if (x < CircleSize) {
				m_mouseX = x - CircleSize/2;
				m_mouseY = y - CircleSize/2;
				// when moving started
				if (!m_mouseMoving) {
					m_lastMouseX = m_mouseX;
					m_lastMouseY = m_mouseY;
				}
				newMoving = true;
			} else if (x >= HostVideo::Width-CircleSize) {
				x -= HostVideo::Width-CircleSize;
				m_buttons |= buttonsInCircle(x, y) << 4;
				// swap bits
				int left   = (m_buttons & 2) >> 1;
				int right  = (m_buttons & 1) >> 0;
				int middle = (m_buttons & 4) >> 2;
				m_buttons = (left << 0) | (right << 1) | (middle << 2);
			}
		}
	}
	m_mouseMoving = newMoving;
}

int TouchInputDevice::buttonsInCircle(int x, int y) const {
	int buttons = 0;
	if (x < CircleSize/4) {
		buttons |= Emu::PadKey_Left;
		if (y < CircleSize/4)
			buttons |= Emu::PadKey_Up;
		else if (y >= CircleSize/4*3)
			buttons |= Emu::PadKey_Down;
	} else if (x >= CircleSize/4*3) {
		buttons |= Emu::PadKey_Right;
		if (y < CircleSize/4)
			buttons |= Emu::PadKey_Up;
		else if (y >= CircleSize/4*3)
			buttons |= Emu::PadKey_Down;
	} else {
		if (y < CircleSize/4) {
			buttons |= Emu::PadKey_Up;
		} else if (y >= CircleSize/4*3) {
			buttons |= Emu::PadKey_Down;
		} else {
			x -= CircleSize/2;
			y -= CircleSize/2;
			if (qAbs(x) > qAbs(y)) {
				if (x > 0)
					buttons |= Emu::PadKey_Right;
				else
					buttons |= Emu::PadKey_Left;
			} else {
				if (y > 0)
					buttons |= Emu::PadKey_Down;
				else
					buttons |= Emu::PadKey_Up;
			}
		}
	}
	return buttons;
}

void TouchInputDevice::paint(QPainter *painter) {
	// pause,exit
	painter->drawImage(0, 0, m_padImage,
					   256, 128+64, ButtonWidth, ButtonHeight);
	painter->drawImage(HostVideo::Width-ButtonWidth, 0, m_padImage,
					   256+80, 128+64, ButtonWidth, ButtonHeight);

	if (emuFunction() <= 0)
		return;

	if (emuFunction() <= 2) {
		// l1,r1
		painter->drawImage(0, 120, m_padImage,
						   256, 256, ButtonWidth, ButtonHeight);
		painter->drawImage(HostVideo::Width-ButtonWidth, 120, m_padImage,
						   256+80, 256, ButtonWidth, ButtonHeight);

		// select, start
		painter->drawImage(HostVideo::Width/2-ButtonWidth,
						   HostVideo::Height-ButtonHeight,
						   m_padImage,
						   256, 128, ButtonWidth*2, ButtonHeight);
	}
	// left and right circle
	painter->translate(0, HostVideo::Height-CircleSize);
	paintCircle(painter, (m_buttons >> 0) & 0x0F);
	painter->translate(HostVideo::Width-CircleSize, 0);
	paintCircle(painter, (m_buttons >> 4) & 0x0F);
}

void TouchInputDevice::paintCircle(QPainter *painter, int buttons) {
	painter->drawImage(0, 48, m_padImage,
					   0, ((buttons & Emu::PadKey_Left) ? 256 : 0)+48, 64, 240-48*2);
	painter->drawImage(0, 0, m_padImage,
					   0, (buttons & Emu::PadKey_Up) ? 256 : 0, 240, 48);
	painter->drawImage(240-64, 48, m_padImage,
					   240-64, ((buttons & Emu::PadKey_Right) ? 256 : 0)+48, 64, 240-48*2);
	painter->drawImage(0, 240-48, m_padImage,
					   0, ((buttons & Emu::PadKey_Down) ? 256 : 0)+240-48, 240, 48);
}
