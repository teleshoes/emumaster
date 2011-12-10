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
#include <QCoreApplication>

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

/** Configures QML window. */
void DiskGallery::setupQml() {
	engine()->addImageProvider("disk", new DiskImageProvider());

	QDeclarativeContext *context = rootContext();
	context->setContextProperty("diskListModel", m_diskListModel);
	context->setContextProperty("diskGallery", this);
	context->setContextProperty("appVersion", QCoreApplication::applicationVersion());

	QObject::connect(engine(), SIGNAL(quit()), SLOT(close()));
	QString qmlPath = QString("%1/qml/gallery/main.qml")
			.arg(PathManager::instance()->installationDirPath());
	setSource(QUrl::fromLocalFile(qmlPath));

	if (m_runCount <= 10)
		QMetaObject::invokeMethod(this, "showFirstRunMsg", Qt::QueuedConnection);
	else
		QMetaObject::invokeMethod(this, "emitDiskUpdate", Qt::QueuedConnection);
}

/** Increments emumaster execute counter. */
void DiskGallery::incrementRunCount() {
	m_settings.beginGroup("diskgallery");
	m_runCount = m_settings.value("runCount", 0).toInt() + 1;
	m_settings.setValue("runCount", m_runCount);
	m_settings.endGroup();
	m_settings.sync();
}

/** Returns how many times emumaster was executed. */
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
		args << "-noAutoSaveLoad";
#if defined(MEEGO_EDITION_HARMATTAN)
	process.startDetached("/usr/bin/single-instance", args);
#elif defined(Q_WS_MAEMO_5)
	QString app = args.takeFirst();
	process.startDetached(app, args);
#endif
}

/** Starts web browser with PayPal address for donation. */
void DiskGallery::donate() {
	QStringList args;
	args << "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=WUG37X8GMW9PQ&lc=US&item_number=emumaster&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted";
	QProcess::startDetached("grob", args);
}

/** Starts web browser with blog address. */
void DiskGallery::homepage() {
	QStringList args;
	args << "http://elemental-mk.blogspot.com";
	QProcess::startDetached("grob", args);
}

/** Starts web browser with wiki address. */
void DiskGallery::wiki() {
	QStringList args;
	args << "http://bitbucket.org/elemental/emumaster";
	QProcess::startDetached("grob", args);
}

/** Emitted on start if the phone uses USB mass storage. */
void DiskGallery::emitDiskUpdate() {
	if (!QFile::exists(PathManager::instance()->diskDirPath("nes"))) {
		emit detachUsb();
		return;
	}
	emit diskUpdate();
}

/** Received from one of emulated systems. Decodes the packet,
	and updates the screen shot. */
void DiskGallery::receiveDatagram() {
	QByteArray ba(m_sock.pendingDatagramSize(), Qt::Uninitialized);
	m_sock.readDatagram(ba.data(), ba.size());
	QDataStream s(&ba, QIODevice::ReadOnly);
	QString diskFileName;
	s >> diskFileName;
	m_diskListModel->updateScreenShot(diskFileName);
}

/** Starts SixAxis Monitor app. */
void DiskGallery::sixAxisMonitor() {
	QStringList args;
	args << (PathManager::instance()->installationDirPath() + "/bin/sixaxismonitor");
	QProcess::startDetached("/usr/bin/single-instance", args);
}

QVariant DiskGallery::globalOption(const QString &name, const QVariant &defaultValue) {
	return m_settings.value(name, defaultValue);
}

void DiskGallery::setGlobalOption(const QString &name, const QVariant &value) {
	m_settings.setValue(name, value);
	m_settings.sync();
}
