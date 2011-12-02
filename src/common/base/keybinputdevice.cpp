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

#include "keybinputdevice.h"

KeybInputDevice::KeybInputDevice(QObject *parent) :
	HostInputDevice("keyb", parent) {

	QStringList confList;
	confList << tr("None");
	confList << tr("Pad A");
	confList << tr("Pad B");
	confList << tr("Keyboard");
	setConfList(confList);
}

/*void HostInput::processKey(Qt::Key key, bool state) {
	int lastKeys = m_keysPhone;
	switch (key) {
	case Qt::Key_Left:	setKeyState(IMachine::Left_PadKey, state); break;
	case Qt::Key_Right:	setKeyState(IMachine::Right_PadKey, state); break;
	case Qt::Key_Up:	setKeyState(IMachine::Up_PadKey, state); break;
	case Qt::Key_Down:	setKeyState(IMachine::Down_PadKey, state); break;
	case Qt::Key_C:		setKeyState(IMachine::A_PadKey, state); break;
	case Qt::Key_X:		setKeyState(IMachine::B_PadKey, state); break;
	case Qt::Key_S:		setKeyState(IMachine::X_PadKey, state); break;
	case Qt::Key_D:		setKeyState(IMachine::Y_PadKey, state); break;
	case Qt::Key_Q:		setKeyState(IMachine::Start_PadKey, state); break;
	case Qt::Key_G:		setKeyState(IMachine::L_PadKey, state); break;
	case Qt::Key_H:		setKeyState(IMachine::R_PadKey, state); break;
	case Qt::Key_T:		setKeyState(IMachine::L2_PadKey, state); break;
	case Qt::Key_Z:		setKeyState(IMachine::R2_PadKey, state); break;
	case Qt::Key_W:		setKeyState(IMachine::Select_PadKey, state); break;
	case Qt::Key_Escape:emit pause(); break;
	default: break;
	}
	if (m_keysPhone != lastKeys)
		m_machine->setPadKeys(0, m_keysPhone);
}*/
