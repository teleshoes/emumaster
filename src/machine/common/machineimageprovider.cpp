#include "machineimageprovider.h"
#include "machineview.h"
#include "machinestatelistmodel.h"
#include "hostvideo.h"

MachineImageProvider::MachineImageProvider(MachineView *machineView, MachineStateListModel *stateListModel) :
	QDeclarativeImageProvider(Image),
	m_machineView(machineView),
	m_stateListModel(stateListModel) {
}

QImage MachineImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
	Q_UNUSED(size)
	Q_UNUSED(requestedSize)
	QImage result;
	if (id.startsWith("screenShotGrayscaled")) {
		result = m_machineView->m_hostVideo->screenShotGrayscaled();
	} else {
		QString idGoodPart = id.left(id.indexOf('*'));
		bool ok;
		int i = idGoodPart.toInt(&ok);
		if (ok)
			result = m_stateListModel->screenShot(i);
	}
	return result;
}
