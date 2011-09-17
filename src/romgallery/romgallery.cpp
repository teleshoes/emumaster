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

void RomGallery::launch(const QString &diskName, bool autoload) {
	QProcess process;
	QString machineName = m_romListModel->machineName();
	QStringList args;
	args << QString("%1/bin/%2").arg(IMachine::installationDirPath()).arg(machineName);
	args << diskName;
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

bool RomGallery::addIconToHomeScreen(const QString &diskName, qreal scale, int x, int y) {
	QString machineName = m_romListModel->machineName();
	RomImageProvider imgProvider;
	QImage imgSrc = imgProvider.requestImage(QString("%1_%2*%3")
											 .arg(m_romListModel->machineName())
											 .arg(diskName)
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
	homeScreenIconPaths(&desktopPath, &iconPath, diskName);
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
			.arg(diskName)
			.arg(iconPath);

	QFile file(desktopPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	QTextStream out(&file);
	out << desktopFileContent;
	file.close();
	return true;
}

void RomGallery::removeIconFromHomeScreen(const QString &diskName) {
	QString desktopPath, iconPath;
	homeScreenIconPaths(&desktopPath, &iconPath, diskName);
	QFile desktopFile(desktopPath);
	QFile iconFile(iconPath);
	desktopFile.remove();
	iconFile.remove();
}

bool RomGallery::iconInHomeScreenExists(const QString &diskName) {
	QString desktopPath;
	homeScreenIconPaths(&desktopPath, 0, diskName);
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

void RomGallery::homeScreenIconPaths(QString *desktopFilePath, QString *iconFilePath, const QString &diskName) {
	QString machineName = m_romListModel->machineName();
	QString escapedDiskName = diskName;
	escapedDiskName.replace(' ', '_');

	if (iconFilePath) {
		*iconFilePath = QString("%1/icon/%2_%3.png")
				.arg(IMachine::userDataDirPath())
				.arg(machineName)
				.arg(escapedDiskName);
	}
	if (desktopFilePath) {
		*desktopFilePath = QString("%1/.local/share/applications/emumaster_%2_%3.desktop")
				.arg(getenv("HOME"))
				.arg(machineName)
				.arg(escapedDiskName);
	}
}
