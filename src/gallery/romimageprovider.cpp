#include "romimageprovider.h"
#include <QImage>
#include <QPainter>
#include <QFile>

RomImageProvider::RomImageProvider() :
	QDeclarativeImageProvider(Image),
	m_noScreenShot(256, 256, QImage::Format_ARGB32) {
	QPainter painter;
	painter.begin(&m_noScreenShot);
	painter.fillRect(QRectF(QPointF(), m_noScreenShot.size()), QColor(qRgb(0x4B, 0x4A, 0x4C)));
	painter.setPen(qRgb(0xE0, 0xE1, 0xE2));
	painter.translate(180, -60);
	painter.rotate(60);
	QFont font = painter.font();
	font.setBold(true);
	font.setPixelSize(35);
	painter.setFont(font);
	QTextOption option;
	option.setAlignment(Qt::AlignCenter);
	painter.drawText(m_noScreenShot.rect(), "NO SCREENSHOT", option);
	painter.end();
}

QImage RomImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
	Q_UNUSED(size)
	Q_UNUSED(requestedSize)
	QString path = QString("../data/%1.jpg").arg(id);
	if (!QFile::exists(path))
		return m_noScreenShot;
	QImage img;
	if (!img.load(path))
		return m_noScreenShot;
	return img;
}
