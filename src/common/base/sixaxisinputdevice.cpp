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

#include "sixaxisinputdevice.h"
#include <sixaxis.h>

SixAxisInputDevice::SixAxisInputDevice(SixAxis *sixAxis, QObject *parent) :
	HostInputDevice("sixaxis", parent),
	m_sixAxis(sixAxis) {

	QObject::connect(m_sixAxis, SIGNAL(onSixAxisUpdated()), SLOT(onSixAxisUpdated()));
	QObject::connect(m_sixAxis, SIGNAL(disconnected()), SLOT(deleteLater()));
	QObject::connect(this, SIGNAL(confChanged()), SLOT(onConfChanged()));

	QStringList confList;
	confList << tr("None");
	confList << tr("Pad A");
	confList << tr("Pad B");
	confList << tr("Mouse A");
	confList << tr("Mouse B");
	confList << tr("Pad B + Mouse A");
	confList << tr("Pad A + Mouse B");
	setConfList(confList);
}

const int SixAxisInputDevice::m_buttonsMapping[] = {
	Select_PadKey,
	0,
	0,
	Start_PadKey,
	Up_PadKey,
	Right_PadKey,
	Down_PadKey,
	Left_PadKey,
	L2_PadKey,
	R2_PadKey,
	L_PadKey,
	R_PadKey,
	X_PadKey,
	A_PadKey,
	B_PadKey,
	Y_PadKey
};

void SixAxisInputDevice::onConfChanged() {
	m_converted = false;
	m_buttons = 0;
	m_mouseX = m_mouseY = 0;
	m_mouseButtons = 0;
}

void SixAxisInputDevice::onSixAxisUpdated() {
	m_converted = false;
}

void SixAxisInputDevice::convertPad() {
	int b = m_sixAxis->buttons();
	if (b & (1<<SixAxis::PS)) {
		emit pause();
		return;
	}
	m_buttons = 0;
	for (uint i = 0; i < sizeof(m_buttonsMapping)/sizeof(int); i++) {
		if (b & (1 << i))
			m_buttons |= m_buttonsMapping[i];
	}
}

void SixAxisInputDevice::convertMouse() {
	m_mouseButtons = 0;

	if (m_sixAxis->buttons() & (1<<SixAxis::L2))
		m_mouseButtons |= 1;
	if (m_sixAxis->buttons() & (1<<SixAxis::R2))
		m_mouseButtons |= 2;

	m_mouseX = m_sixAxis->axis(SixAxis::LX);
	m_mouseY = m_sixAxis->axis(SixAxis::LY);

	// deadzones
	if (qAbs(m_mouseX) < 30)
		m_mouseX = 0;
	if (qAbs(m_mouseY) < 30)
		m_mouseY = 0;
}

void SixAxisInputDevice::update(int *data) {
	if (confIndex() <= 0)
		return;

	int pad = -1;
	if (confIndex() <= 2)
		pad = confIndex()-1;
	else if (confIndex() >= 5)
		pad = 6-confIndex();

	int mouse = -1;
	if (confIndex() >= 3 && confIndex() <= 6)
		mouse = confIndex() - (confIndex() & ~1);

	if (!m_converted) {
		if (pad >= 0)
			convertPad();
		if (mouse >= 0)
			convertMouse();
		m_converted = true;
	}

	if (pad >= 0) {
		int *padData = padOffset(data, pad);
		*padData |= m_buttons;
	}
	if (mouse >= 0) {
		int *mouseData = mouseOffset(data, mouse);
		mouseData[0] = m_mouseButtons;
		mouseData[1] = m_mouseX;
		mouseData[2] = m_mouseY;
	}
}
