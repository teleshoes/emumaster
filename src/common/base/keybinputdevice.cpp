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

// TODO configurable keyboard
// TODO when lost focus clear buttons

#include "keybinputdevice.h"
#include "imachine.h"
#include <QSettings>
#include <QDataStream>

KeybInputDevice::KeybInputDevice(QObject *parent) :
	HostInputDevice("keyb", parent) {
	QObject::connect(this, SIGNAL(confChanged()), SLOT(onConfChanged()));

	QHash<int, int> defaultMapping;
	defaultMapping[Qt::Key_Left] = IMachine::PadKey_Left;
	defaultMapping[Qt::Key_Right] = IMachine::PadKey_Right;
	defaultMapping[Qt::Key_Up] = IMachine::PadKey_Up;
	defaultMapping[Qt::Key_Down] = IMachine::PadKey_Down;

	defaultMapping[Qt::Key_C] = IMachine::PadKey_A;
	defaultMapping[Qt::Key_X] = IMachine::PadKey_B;
	defaultMapping[Qt::Key_S] = IMachine::PadKey_X;
	defaultMapping[Qt::Key_D] = IMachine::PadKey_Y;

	defaultMapping[Qt::Key_Q] = IMachine::PadKey_Start;
	defaultMapping[Qt::Key_W] = IMachine::PadKey_Select;

	defaultMapping[Qt::Key_G] = IMachine::PadKey_L1;
	defaultMapping[Qt::Key_H] = IMachine::PadKey_R1;
	defaultMapping[Qt::Key_T] = IMachine::PadKey_L2;
	defaultMapping[Qt::Key_Z] = IMachine::PadKey_R2;

	QByteArray ba;

	QDataStream *stream = new QDataStream(&ba, QIODevice::WriteOnly);
	*stream << defaultMapping;
	delete stream;

	QSettings s;
	ba = s.value("keybMapping", ba).toByteArray();

	stream = new QDataStream(&ba, QIODevice::ReadOnly);
	*stream >> m_mapping;
	delete stream;
}

void KeybInputDevice::onConfChanged() {
	m_buttons = 0;
	m_keys.clear();
}

void KeybInputDevice::update(int *data) {
	if (confIndex() <= 0)
		return;

	if (confIndex() <= 2) {
		int *pad = IMachine::padOffset(data, confIndex()-1);
		pad[0] = m_buttons;
	} else if (confIndex() == 3) {
		while (!m_keys.isEmpty())
			IMachine::keybEnqueue(data, m_keys.takeFirst());
	}
}

void KeybInputDevice::processKey(Qt::Key key, bool state) {
	if (confIndex() <= 0)
		return;

	if (confIndex() <= 2) {
		int button = m_mapping[key];
		if (state)
			m_buttons |=  button;
		else
			m_buttons &= ~button;
	} else if (confIndex() == 3) {
		int k = key;
		if (!state)
			k |= (1 << 31);
		m_keys.append(k);
	}
}
