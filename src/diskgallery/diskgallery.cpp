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

#include "diskgallery.h"
#include "disklistmodel.h"
#include "diskimageprovider.h"
#include <pathmanager.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFile>
#include <QProcess>
#include <QSettings>

DiskGallery::DiskGallery(QWidget *parent) :
	QDeclarativeView(parent) {
	m_diskListModel = new DiskListModel(this);
	incrementRunCount();
	setupQml();

	m_sock.bind(QHostAddress::LocalHost, 5798);
	QObject::connect(&m_sock, SIGNAL(readyRead()), SLOT(receiveDatagram()));
}

DiskGallery::~DiskGallery() {
}

void DiskGallery::setupQml() {
	engine()->addImageProvider("disk", new DiskImageProvider());
	rootContext()->setContextProperty("diskListModel", m_diskListModel);
	rootContext()->setContextProperty("diskGallery", this);
	QObject::connect(engine(), SIGNAL(quit()), SLOT(close()));
	QString qmlPath = QString("%1/qml/gallery/main.qml")
			.arg(PathManager::instance()->installationDirPath());
	setSource(QUrl::fromLocalFile(qmlPath));

	if (m_runCount <= 10)
		QMetaObject::invokeMethod(this, "showFirstRunMsg", Qt::QueuedConnection);
	else
		QMetaObject::invokeMethod(this, "emitDiskUpdate", Qt::QueuedConnection);
}

void DiskGallery::incrementRunCount() {
	QSettings s("elemental", "emumaster");
	s.beginGroup("diskgallery");
	m_runCount = s.value("runCount", 0).toInt() + 1;
	s.setValue("runCount", m_runCount);
	s.endGroup();
}

int DiskGallery::runCount() const {
	return m_runCount;
}

void DiskGallery::launch(int index, bool autoload) {
	QString diskFileName = m_diskListModel->getDiskFileName(index);
	QString diskMachine = m_diskListModel->getDiskMachine(index);
	if (diskFileName.isEmpty())
		return;
	QProcess process;
	QStringList args;
	args << QString("%1/bin/%2")
			.arg(PathManager::instance()->installationDirPath())
			.arg(diskMachine);
	args << diskFileName;
	if (!autoload)
		args << "-noautoload";
#if defined(MEEGO_EDITION_HARMATTAN)
	process.startDetached("/usr/bin/single-instance", args);
#elif defined(Q_WS_MAEMO_5)
	QString app = args.takeFirst();
	process.startDetached(app, args);
#endif
}

void DiskGallery::donate() {
	QStringList args;
	args << "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=WUG37X8GMW9PQ&lc=US&item_number=emumaster&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted";
	QProcess::startDetached("grob", args);
}

void DiskGallery::homepage() {
	QStringList args;
	args << "http://elemental-mk.blogspot.com";
	QProcess::startDetached("grob", args);
}

void DiskGallery::emitDiskUpdate() {
	if (!QFile::exists(PathManager::instance()->diskDirPath("nes"))) {
		emit detachUsb();
		return;
	}
	emit diskUpdate();
}

void DiskGallery::receiveDatagram() {
	QByteArray ba(m_sock.pendingDatagramSize(), Qt::Uninitialized);
	m_sock.readDatagram(ba.data(), ba.size());
	QDataStream s(&ba, QIODevice::ReadOnly);
	QString diskFileName;
	s >> diskFileName;
	m_diskListModel->updateScreenShot(diskFileName);
}

void DiskGallery::sixAxisMonitor() {
	QStringList args;
	args << (PathManager::instance()->installationDirPath() + "/bin/sixaxismonitor");
	QProcess::startDetached("/usr/bin/single-instance", args);
}
