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
#include <pathmanager.h>
#include <QTimer>
#include <QtDeclarative>
#include <QApplication>

/**
	\class SixAxisMonitor
	QML window to show detected PS3 controllers.
 */

/** Creates SixAxisMonitor. */
SixAxisMonitor::SixAxisMonitor() {
	m_identifyDev = 0;
	m_server = new SixAxisServer(this);
	QObject::connect(m_server, SIGNAL(countChanged()), SIGNAL(addressesChanged()));

	rootContext()->setContextProperty("sixAxisMonitor", this);

	QString qmlSrcPath = QString("%1/qml/sixaxismonitor/main.qml")
			.arg(PathManager::instance()->installationDirPath());
	setSource(QUrl::fromLocalFile(qmlSrcPath));
}

/**
	Starts listening for new PS3 controllers.
	Returns non-empty string on error.
 */
QString SixAxisMonitor::start() {
	return m_server->open();
}

/** Starts the animation sequence of LEDs in sixaxis. */
void SixAxisMonitor::identify(int i) {
	if (m_identifyDev)
		return;
	if (i < 0 || i >= m_server->numDevices())
		return;

	SixAxisDevice *dev = m_server->device(i);
	QObject::connect(dev, SIGNAL(destroyed()),
					 SLOT(onIdentifyDevDestroyed()));
	m_identifyDev = dev;
	m_identifyOldLeds = dev->leds();
	m_identifyCounter = 0;
	onIdentifyEvent();
}

/** Disconnects sixaxis with ID \a i. */
void SixAxisMonitor::disconnectDev(int i) {
	if (i < 0 || i >= m_server->numDevices())
		return;
	SixAxisDevice *dev = m_server->device(i);
	m_server->disconnectDevice(dev);
}

/** Called when device was destroyed while identifying. */
void SixAxisMonitor::onIdentifyDevDestroyed() {
	m_identifyDev = 0;
}

/** Advances LED animation. */
void SixAxisMonitor::onIdentifyEvent() {
	if (!m_identifyDev)
		return;

	static const int LedPatterns[] = { 1, 2, 4, 8, 4, 2, 1, 0 };
	static const int NumOfPatterns = sizeof(LedPatterns);

	int leds = LedPatterns[m_identifyCounter];

	m_identifyCounter++;
	if (m_identifyCounter == NumOfPatterns)
		leds = m_identifyOldLeds;

	m_identifyDev->setLeds(leds);

	if (m_identifyCounter == NumOfPatterns) {
		QObject::disconnect(m_identifyDev, SIGNAL(destroyed()),
							this, SLOT(onIdentifyDevDestroyed()));
		m_identifyDev = 0;
	} else {
		QTimer::singleShot(300, this, SLOT(onIdentifyEvent()));
	}
}

/** Returns the list of addresses of connected controllers. */
QStringList SixAxisMonitor::addresses() const {
	QStringList result;
	for (int i = 0; i < m_server->numDevices(); i++)
		result << m_server->device(i)->addressString();
	return result;
}

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	SixAxisMonitor view;
	view.showFullScreen();
	return app.exec();
}
