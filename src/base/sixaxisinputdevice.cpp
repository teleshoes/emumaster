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
#include "emu.h"
#include <sixaxis.h>

// TODO check mouse

SixAxisInputDevice::SixAxisInputDevice(SixAxis *sixAxis, QObject *parent) :
	HostInputDevice("sixaxis", QObject::tr("SixAxis"), parent),
	m_sixAxis(sixAxis)
{
	QStringList functionNameList;
	functionNameList << tr("None");
	functionNameList << tr("Pad A");
	functionNameList << tr("Pad B");
	functionNameList << tr("Mouse A");
	functionNameList << tr("Mouse B");
	functionNameList << tr("Pad B + Mouse A");
	functionNameList << tr("Pad A + Mouse B");
	setEmuFunctionNameList(functionNameList);

	QObject::connect(m_sixAxis, SIGNAL(updated()), SLOT(onSixAxisUpdated()));
	QObject::connect(m_sixAxis, SIGNAL(disconnected()), SLOT(deleteLater()));
	QObject::connect(this, SIGNAL(emuFunctionChanged()), SLOT(onConfChanged()));

	m_sixAxis->setParent(this);
}

const int SixAxisInputDevice::m_buttonsMapping[] = {
	Emu::PadKey_Select,
	0,
	0,
	Emu::PadKey_Start,
	Emu::PadKey_Up,
	Emu::PadKey_Right,
	Emu::PadKey_Down,
	Emu::PadKey_Left,
	Emu::PadKey_L2,
	Emu::PadKey_R2,
	Emu::PadKey_L1,
	Emu::PadKey_R1,
	Emu::PadKey_X,
	Emu::PadKey_A,
	Emu::PadKey_B,
	Emu::PadKey_Y
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

	if (m_sixAxis->buttons() & (1<<SixAxis::L3))
		m_mouseButtons |= 1;
	if (m_sixAxis->buttons() & (1<<SixAxis::R3))
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
	if (emuFunction() <= 0)
		return;

	int pad = -1;
	if (emuFunction() <= 2)
		pad = emuFunction()-1;
	else if (emuFunction() >= 5)
		pad = 6-emuFunction();

	int mouse = -1;
	if (emuFunction() >= 3 && emuFunction() <= 6)
		mouse = emuFunction() - (emuFunction() & ~1);

	if (!m_converted) {
		if (pad >= 0)
			convertPad();
		if (mouse >= 0)
			convertMouse();
		m_converted = true;
	}

	if (pad >= 0) {
		int *padData = Emu::padOffset(data, pad);
		padData[0] |= m_buttons;
	}
	if (mouse >= 0) {
		int *mouseData = Emu::mouseOffset(data, mouse);
		mouseData[0] = m_mouseButtons;
		mouseData[1] = m_mouseX;
		mouseData[2] = m_mouseY;
	}
}
