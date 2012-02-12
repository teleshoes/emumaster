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
#include "configuration.h"

HostInputDevice::HostInputDevice(const QString &name, QObject *parent) :
	QObject(parent),
	m_name(name),
	m_confIndex(0)
{
}

void HostInputDevice::setConfIndex(int index)
{
	if (index < 0)
		index = 0;
	if (index != m_confIndex) {
		if (!m_globalConfName.isEmpty())
			emConf.setValue(m_globalConfName, index);
		m_confIndex = index;
		emit confChanged();
	}
}

void HostInputDevice::setGlobalConfigurationName(const QString &name)
{
	m_globalConfName = name;
}

void HostInputDevice::updateConfFromGlobalConfiguration()
{
	int conf = emConf.value(m_globalConfName, -1).toInt();
	if (conf >= 0)
		setConfIndex(conf);
}

/*!
	\fn HostInputDevice::update(int *data)
	Writes data to the emulated systems.
	The data depends on the chosen configuration.
*/
