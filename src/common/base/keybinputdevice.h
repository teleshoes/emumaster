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

#ifndef KEYBINPUTDEVICE_H
#define KEYBINPUTDEVICE_H

#include "hostinputdevice.h"
#include <QHash>

class KeybInputDevice : public HostInputDevice {
    Q_OBJECT
public:
    explicit KeybInputDevice(QObject *parent = 0);
	void update(int *data);
	void processKey(Qt::Key key, bool state);
private slots:
	void onConfChanged();
private:
	QHash<int, int> m_mapping;
	int m_buttons;
	QList<int> m_keys;
};

#endif // KEYBINPUTDEVICE_H
