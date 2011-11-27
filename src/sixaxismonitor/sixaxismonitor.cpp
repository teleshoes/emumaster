/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "sixaxismonitor.h"
#include "sixaxisserver.h"
#include "sixaxisdevice.h"
#include <QTimer>
#include <QtDeclarative>

SixAxisMonitor::SixAxisMonitor() {
	m_identifyDev = 0;
	m_server = new SixAxisServer(this);
	QObject::connect(m_server, SIGNAL(countChanged()), SIGNAL(addressesChanged()));

	rootContext()->setContextProperty("sixAxisMonitor", this);
	setSource(QUrl::fromLocalFile("/opt/emumaster/qml/sixaxismonitor/main.qml"));
}

QString SixAxisMonitor::start() {
	return m_server->open();
}

void SixAxisMonitor::identify(int i) {
	if (m_identifyDev)
		return;
	if (i < m_server->numDevices()) {
		SixAxisDevice *dev = m_server->device(i);
		QObject::connect(dev, SIGNAL(destroyed()), SLOT(identifyDevDestroyed()));
		m_identifyDev = dev;
		m_identifyOldLeds = dev->leds();
		m_identifyCounter = 0;
		identifyEvent();
	}
}

void SixAxisMonitor::disconnectDev(int i) {
	if (i < m_server->numDevices()) {
		SixAxisDevice *dev = m_server->device(i);
		m_server->disconnectDevice(dev);
	}
}

void SixAxisMonitor::identifyDevDestroyed() {
	m_identifyDev = 0;
}

void SixAxisMonitor::identifyEvent() {
	if (!m_identifyDev)
		return;

	static const int patterns[] = { 1, 2, 4, 8, 4, 2, 1, 0 };
	int leds = patterns[m_identifyCounter];

	m_identifyCounter++;
	if (m_identifyCounter == 8)
		leds = m_identifyOldLeds;

	m_identifyDev->setLeds(leds);

	if (m_identifyCounter == 8)
		m_identifyDev = 0;
	else
		QTimer::singleShot(300, this, SLOT(identifyEvent()));
}

QStringList SixAxisMonitor::addresses() const {
	QStringList result;
	for (int i = 0; i < m_server->numDevices(); i++)
		result << m_server->device(i)->addressString();
	return result;
}
