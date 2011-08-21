#include "machineimageprovider.h"
#include "machineview.h"
#include "hostvideo.h"

MachineImageProvider::MachineImageProvider(MachineView *machineView) :
	QDeclarativeImageProvider(Image),
	m_machineView(machineView) {
}

QImage MachineImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
	QImage result;
	if (id.startsWith("screenShotGrayscaled")) {
		return m_machineView->m_hostVideo->screenShotGrayscaled();
	}
	return result;
}
