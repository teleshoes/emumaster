#include "disklistmodel.h"
#include <pathmanager.h>
#include "diskimageprovider.h"
#include <QPainter>
#include <QTextStream>

QImage DiskListModel::applyMaskAndOverlay(const QImage &icon) {
	QString iconMaskPath = QString("%1/data/icon_mask.png")
			.arg(PathManager::instance()->installationDirPath());
	QString iconOverlayPath = QString("%1/data/icon_overlay.png")
			.arg(PathManager::instance()->installationDirPath());

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

bool DiskListModel::addIconToHomeScreen(int i, qreal scale, int x, int y) {
	QString diskFileName = getDiskFileName(i);
	QString diskTitle = getDiskTitle(i);
	QString diskMachine = getDiskMachine(i);
	DiskImageProvider imgProvider;
	QImage imgSrc = imgProvider.requestImage(QString("%1/%2*%3")
											 .arg(diskMachine)
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

	QString desktopFilePath = PathManager::instance()->desktopFilePath(diskMachine, diskTitle);
	QString iconFilePath = PathManager::instance()->homeScreenIconPath(diskMachine, diskTitle);
	if (!icon.save(iconFilePath))
		return false;
	QString exec = QString(
		#if defined(MEEGO_EDITION_HARMATTAN)
			 "/usr/bin/single-instance %1/bin/%2 \"%3\"")
		#elif defined(Q_WS_MAEMO_5)
			"%1/bin/%2 \"%3\"")
		#endif
			.arg(PathManager::instance()->installationDirPath())
			.arg(diskMachine)
			.arg(diskFileName);
	return createDesktopFile(desktopFilePath,
							 diskTitle,
							 exec,
							 iconFilePath,
							 "Game;Emulator;");
}

bool DiskListModel::createDesktopFile(const QString &fileName,
									 const QString &title,
									 const QString &exec,
									 const QString &icon,
									 const QString &categories) {
	QString desktopFileContent = QString(
				"[Desktop Entry]\n"
				"Version=1.0\n"
				"Type=Application\n"
				"Name=%1\n"
				"Exec=%2\n"
				"Icon=%3\n"
				"Terminal=false\n"
				"Categories=%4\n")
			.arg(title)
			.arg(exec)
			.arg(icon)
			.arg(categories);

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	QTextStream out(&file);
	out << desktopFileContent;
	file.close();
	return true;
}

void DiskListModel::removeIconFromHomeScreen(int i) {
	QString diskTitle = getDiskTitle(i);
	QString diskMachine = getDiskMachine(i);
	if (diskMachine.isEmpty())
		return;

	QFile desktopFile(PathManager::instance()->desktopFilePath(diskMachine, diskTitle));
	QFile iconFile(PathManager::instance()->homeScreenIconPath(diskMachine, diskTitle));
	desktopFile.remove();
	iconFile.remove();
}

bool DiskListModel::iconInHomeScreenExists(int i) {
	QString diskTitle = getDiskTitle(i);
	QString diskMachine = getDiskMachine(i);
	if (diskMachine.isEmpty())
		return false;
	QString path = PathManager::instance()->desktopFilePath(diskMachine, diskTitle);
	return QFile::exists(path);
}
