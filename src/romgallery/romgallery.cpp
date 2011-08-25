#include "romgallery.h"
#include "romimageprovider.h"
#include "romlistmodel.h"
#include "imachine.h"
#include "machineview.h"
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QTimer>
#include <QCloseEvent>
#include <QDir>
#include <QFile>
#include <QTextStream>

RomGallery::RomGallery(QWidget *parent) :
	QDeclarativeView(parent),
	m_machineView(0) {
	m_romListModel = new RomListModel(this);
	engine()->addImageProvider("rom", new RomImageProvider());
	rootContext()->setContextProperty("romListModel", m_romListModel);
	rootContext()->setContextProperty("romGallery", this);
	setSource(QUrl::fromLocalFile("../qml/gallery/main.qml"));
}

RomGallery::~RomGallery() {
}

bool RomGallery::launch(const QString &diskName) {
	QString machineName = m_romListModel->machineName();
	IMachine *machine = IMachine::loadMachine(machineName);
	if (!machine)
		return false;
	m_diskName = diskName;
	m_machineView = new MachineView(machine, diskName, this);
	QObject::connect(m_machineView, SIGNAL(destroyed()), SLOT(onMachineViewDestroyed()));
	setVisible(false);
	return true;
}

void RomGallery::onMachineViewDestroyed() {
	m_machineView = 0;
	m_romListModel->updateScreenShot(m_diskName);
	setVisible(true);
}

void RomGallery::closeEvent(QCloseEvent *e) {
	e->setAccepted(m_machineView == 0);
}

QImage RomGallery::applyMaskAndOverlay(const QImage &icon) {
	QImage iconConverted = icon.convertToFormat(QImage::Format_ARGB32);
	QImage result(80, 80, QImage::Format_ARGB32);
	QImage mask;
	mask.load("../data/icon_mask.png");
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
	overlay.load("../data/icon_overlay.png");
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
	QString escapedDiskName = diskName;
	escapedDiskName.replace(' ', '_');
	RomImageProvider imgProvider;
	QImage imgSrc = imgProvider.requestImage(QString("%1%2*%3")
											 .arg(m_romListModel->machineName())
											 .arg(escapedDiskName)
											 .arg(qrand()), 0, QSize());
	QImage scaled = imgSrc.scaled(qreal(imgSrc.width())*scale,
								  qreal(imgSrc.height())*scale,
								  Qt::IgnoreAspectRatio,
								  Qt::SmoothTransformation);
	QImage icon = scaled.copy(x, y, 80, 80);
	if (icon.width() != 80 || icon.height() != 80)
		return false;
	icon = applyMaskAndOverlay(icon);

	QString machineName = m_romListModel->machineName();
	QString iconPath = QString("%1/icon/%2%3.png")
			.arg(MachineView::userDataDirPath())
			.arg(machineName)
			.arg(escapedDiskName);
	if (!icon.save(iconPath))
		return false;

	QString desktopFileContent = QString(
				"[Desktop Entry]\n"
				"Encoding=UTF-8\n"
				"Version=1.0\n"
				"Type=Application\n"
				"Name=%3\n"
				"Exec=/usr/bin/single-instance %1/launcher %2 \"%3\"\n"
				"Icon=%4\n"
				"Terminal=false\n"
				"Categories=Emulator;\n")
			.arg(QDir::currentPath())
			.arg(machineName)
			.arg(escapedDiskName)
			.arg(iconPath);

	QFile file(QString("%1/.local/share/applications/emumaster_%2_%3.desktop")
			   .arg(getenv("HOME"))
			   .arg(machineName)
			   .arg(escapedDiskName));
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	QTextStream out(&file);
	out << desktopFileContent;
	file.close();
	return true;
}
