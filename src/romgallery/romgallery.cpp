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

#include "romgallery.h"
#include "romimageprovider.h"
#include "romlistmodel.h"
#include "imachine.h"
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFile>
#include <QTextStream>
#include <QProcess>

RomGallery::RomGallery(QWidget *parent) :
	QDeclarativeView(parent) {
	m_romListModel = new RomListModel(this);
	engine()->addImageProvider("rom", new RomImageProvider());
	rootContext()->setContextProperty("romListModel", m_romListModel);
	rootContext()->setContextProperty("romGallery", this);
	QObject::connect(engine(), SIGNAL(quit()), SLOT(close()));
	QString qmlPath = QString("%1/qml/gallery/main.qml")
			.arg(IMachine::installationDirPath());
	setSource(QUrl::fromLocalFile(qmlPath));
	QMetaObject::invokeMethod(this, "emitRomUpdate", Qt::QueuedConnection);
}

RomGallery::~RomGallery() {
}

void RomGallery::launch(int index, bool autoload) {
	QString diskFileName = m_romListModel->getDiskFileName(index);
	if (diskFileName.isEmpty())
		return;
	QProcess process;
	QString machineName = m_romListModel->machineName();
	QStringList args;
	args << QString("%1/bin/%2").arg(IMachine::installationDirPath()).arg(machineName);
	args << diskFileName;
	if (!autoload)
		args << "-noautoload";
	process.startDetached("/usr/bin/single-instance", args);
}

QImage RomGallery::applyMaskAndOverlay(const QImage &icon) {
	QString iconMaskPath = QString("%1/data/icon_mask.png")
			.arg(IMachine::installationDirPath());
	QString iconOverlayPath = QString("%1/data/icon_overlay.png")
			.arg(IMachine::installationDirPath());

	QImage iconConverted = icon.convertToFormat(QImage::Format_ARGB32);
	QImage result(80, 80, QImage::Format_ARGB32);
	QImage mask;
	mask.load(iconMaskPath);
	mask = mask.convertToFormat(QImage::Format_ARGB32);
	int pixelCount = mask.width() * mask.height();
	QRgb *data = (QRgb *)mask.bits();
	for (int i = 0; i < pixelCount; ++i) {
		uint val = data[i];
		if (val < qRgb(0x80, 0x80, 0x80))
			data[i] = qRgba(0, 0, 0, 0);
		else
			data[i] = qRgba(255, 255, 255, 255);
	}

	QImage overlay;
	overlay.load(iconOverlayPath);
	overlay = overlay.convertToFormat(QImage::Format_ARGB32);

	QPainter painter;
	painter.begin(&result);
	painter.fillRect(result.rect(), Qt::transparent);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawImage(0, 0, iconConverted);
	painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	painter.drawImage(0, 0, mask);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawImage(0, 0, overlay);
	painter.end();
	return result;
}

bool RomGallery::addIconToHomeScreen(int index, qreal scale, int x, int y) {
	QString diskFileName = m_romListModel->getDiskFileName(index);
	QString diskTitle = m_romListModel->getDiskTitle(index);
	QString machineName = m_romListModel->machineName();
	RomImageProvider imgProvider;
	QImage imgSrc = imgProvider.requestImage(QString("%1_%2*%3")
											 .arg(m_romListModel->machineName())
											 .arg(diskTitle)
											 .arg(qrand()), 0, QSize());
	QImage scaled = imgSrc.scaled(qreal(imgSrc.width())*scale,
								  qreal(imgSrc.height())*scale,
								  Qt::IgnoreAspectRatio,
								  Qt::SmoothTransformation);
	QImage icon = scaled.copy(x, y, 80, 80);
	if (icon.width() != 80 || icon.height() != 80)
		return false;
	icon = applyMaskAndOverlay(icon);

	QString desktopPath, iconPath;
	homeScreenIconPaths(&desktopPath, &iconPath, diskTitle);
	if (!icon.save(iconPath))
		return false;

	QString desktopFileContent = QString(
				"[Desktop Entry]\n"
				"Version=1.0\n"
				"Type=Application\n"
				"Name=%3\n"
				"Exec=/usr/bin/single-instance %1/bin/%2 \"%3\"\n"
				"Icon=%4\n"
				"Terminal=false\n"
				"Categories=Emulator;\n")
			.arg(IMachine::installationDirPath())
			.arg(machineName)
			.arg(diskFileName)
			.arg(iconPath);

	QFile file(desktopPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	QTextStream out(&file);
	out << desktopFileContent;
	file.close();
	return true;
}

void RomGallery::removeIconFromHomeScreen(int index) {
	QString diskTitle = m_romListModel->getDiskTitle(index);
	if (diskTitle.isEmpty())
		return;
	QString desktopPath, iconPath;
	homeScreenIconPaths(&desktopPath, &iconPath, diskTitle);
	QFile desktopFile(desktopPath);
	QFile iconFile(iconPath);
	desktopFile.remove();
	iconFile.remove();
}

bool RomGallery::iconInHomeScreenExists(int index) {
	QString diskTitle = m_romListModel->getDiskTitle(index);
	if (diskTitle.isEmpty())
		return false;
	QString desktopPath;
	homeScreenIconPaths(&desktopPath, 0, diskTitle);
	QFile desktopFile(desktopPath);
	return desktopFile.exists();
}

void RomGallery::donate() {
	QStringList args;
	args << "https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=WUG37X8GMW9PQ&lc=US&item_number=emumaster&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted";
	QProcess::execute("grob", args);
}

void RomGallery::homepage() {
	QStringList args;
	args << "http://elemental-mk.blogspot.com";
	QProcess::execute("grob", args);
}

void RomGallery::emitRomUpdate() {
	if (!QFile::exists(IMachine::diskDirPath("nes"))) {
		emit detachUsb();
		return;
	}
	emit romUpdate();
}

void RomGallery::homeScreenIconPaths(QString *desktopFilePath, QString *iconFilePath, const QString &diskTitle) {
	QString machineName = m_romListModel->machineName();

	if (iconFilePath) {
		*iconFilePath = QString("%1/icon/%2_%3.png")
				.arg(IMachine::userDataDirPath())
				.arg(machineName)
				.arg(diskTitle);
	}
	if (desktopFilePath) {
		*desktopFilePath = QString("%1/.local/share/applications/emumaster_%2_%3.desktop")
				.arg(getenv("HOME"))
				.arg(machineName)
				.arg(diskTitle);
	}
}
