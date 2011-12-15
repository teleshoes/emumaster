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

	QSettings s;
	s.beginGroup("keyboard");
	for (uint i = 0;  i < sizeof(m_defaultMapping)/sizeof(int); i++)
		m_mapping[1<<i] = s.value(QString::number(i), m_defaultMapping[i]).toInt();
	s.endGroup();
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

void KeybInputDevice::processKey(Qt::Key key, bool down) {
	if (confIndex() <= 0)
		return;

	if (confIndex() <= 2) {
		int button = m_mapping[key];
		if (down)
			m_buttons |=  button;
		else
			m_buttons &= ~button;
	} else if (confIndex() == 3) {
		int k = key;
		if (down)
			k |= (1 << 31);
		m_keys.append(k);
	}
}

const int KeybInputDevice::m_defaultMapping[14] = {
	Qt::Key_Right,
	Qt::Key_Down,
	Qt::Key_Up,
	Qt::Key_Left,

	Qt::Key_C,
	Qt::Key_X,
	Qt::Key_S,
	Qt::Key_D,

	Qt::Key_Q,
	Qt::Key_W,

	Qt::Key_G,
	Qt::Key_H,
	Qt::Key_T,
	Qt::Key_Z
};

void KeybInputDevice::setPadKey(int key, int hostKey) {
	QSettings s;
	s.beginGroup("keyboard");
	s.setValue(QString::number(key), hostKey);
	m_mapping[1 << key] = hostKey;
	s.endGroup();
}

int KeybInputDevice::padKey(int key) const {
	return m_mapping[1 << key];
}

void KeybInputDevice::resetToDefaults() {
	QSettings s;
	s.beginGroup("keyboard");
	for (uint i = 0;  i < sizeof(m_defaultMapping)/sizeof(int); i++) {
		s.setValue(QString::number(i), m_defaultMapping[i]);
		m_mapping[1 << i] = m_defaultMapping[i];
	}
	s.endGroup();
}
