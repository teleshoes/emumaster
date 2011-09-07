#include "machineimageprovider.h"
#include "imachine.h"
#include "machinestatelistmodel.h"
#include <QPainter>

MachineImageProvider::MachineImageProvider(IMachine *machine, MachineStateListModel *stateListModel) :
	QDeclarativeImageProvider(Image),
	m_machine(machine),
	m_stateListModel(stateListModel) {
}

QImage MachineImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
	Q_UNUSED(size)
	Q_UNUSED(requestedSize)
	QImage result;
	if (id.startsWith("screenShotGrayscaled")) {
		result = screenShotGrayscaled();
	} else {
		QString idGoodPart = id.left(id.indexOf('*'));
		bool ok;
		int i = idGoodPart.toInt(&ok);
		if (ok)
			result = m_stateListModel->screenShot(i);
	}
	return result;
}

QImage MachineImageProvider::screenShotGrayscaled() const {
	QRect rect(0.0f, 0.0f, 854.0f, 480.0f);
	QRectF srcRect, dstRect;
	srcRect = m_machine->videoSrcRect();
	qreal scale = qMin(854.0f/srcRect.width(), 480.0f/srcRect.height());
	qreal w = srcRect.width() * scale;
	qreal h = srcRect.height() * scale;
	qreal x = 854.0f/2.0f-w/2.0f;
	qreal y = 480.0f/2.0f-h/2.0f;
	dstRect = QRectF(x, y, w, h);

	QImage screenShot(rect.size(), QImage::Format_RGB32);
	QPainter painter;
	painter.begin(&screenShot);
	painter.fillRect(rect, qRgb(0xE0, 0xE1, 0xE2));
	painter.drawImage(dstRect, m_machine->frame(), srcRect);
	painter.end();

	int pixelCount = screenShot.width() * screenShot.height();
	QRgb *data = (QRgb *)screenShot.bits();
	for (int i = 0; i < pixelCount; ++i) {
		QRgb val = data[i];
		uint r = qMin(qRed(val)+0xA0, 0xE0);
		uint g = qMin(qGreen(val)+0xA0, 0xE1);
		uint b = qMin(qBlue(val)+0xA0, 0xE2);
		val = qRgb(r, g, b);
		data[i] = qRgb(val, val, val);
	}
	return screenShot;
}
