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

QString HostInputDevice::conf() const {
	return m_confList.at(m_confIndex);
}

void HostInputDevice::setConf(const QString &type) {
	int i = m_confList.indexOf(type);
	if (i < 0)
		i = 0;
	if (i != m_confIndex) {
		m_confIndex = i;
		emit confChanged();
	}
}

void HostInputDevice::setConfList(const QStringList &list) {
	m_confList = list;
}
