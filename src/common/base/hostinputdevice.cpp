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

#include "hostinputdevice.h"

HostInputDevice::HostInputDevice(const QString &name, QObject *parent) :
	QObject(parent),
	m_name(name),
	m_confIndex(0) {
}

void HostInputDevice::setConfIndex(int index) {
	if (index < 0)
		index = 0;
	if (index != m_confIndex) {
		m_confIndex = index;
		emit confChanged();
	}
}
