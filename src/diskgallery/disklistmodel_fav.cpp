#include "disklistmodel.h"
#include <pathmanager.h>
#include <QDataStream>

void DiskListModel::loadFav() {
	QFile file(QString("%1/favourites").arg(PathManager::instance()->userDataDirPath()));
	if (!file.open(QIODevice::ReadOnly))
		return;
	QDataStream stream(&file);
	stream >> m_favList;
	stream >> m_favListMachine;
}

void DiskListModel::saveFav() {
	QFile file(QString("%1/favourites").arg(PathManager::instance()->userDataDirPath()));
	if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
		return;
	QDataStream stream(&file);
	stream << m_favList;
	stream << m_favListMachine;
}

void DiskListModel::addToFav(int i) {
	Q_ASSERT(m_collection != "fav");
	QString fileName = getDiskFileName(i);
	QString machine = getDiskMachine(i);
	if (machine.isEmpty())
		return;
	int machineId = PathManager::instance()->machines().indexOf(machine);

	int favIndex = m_favList.indexOf(fileName);
	if (favIndex >= 0) {
		if (machineId == m_favListMachine.at(favIndex))
			return;
	}
	m_favList.append(fileName);
	m_favListMachine.append(machineId);
	saveFav();
}

void DiskListModel::removeFromFav(int indexInFavList) {
	if (indexInFavList < 0 || indexInFavList >= m_favList.size())
		return;
	bool favModelCurrent = (m_collection == "fav");
	if (favModelCurrent)
		beginRemoveRows(QModelIndex(), indexInFavList, indexInFavList);
	m_favList.removeAt(indexInFavList);
	m_favListMachine.removeAt(indexInFavList);
	if (favModelCurrent) {
		m_list = m_favList;
		m_listMachine = m_favListMachine;
		endRemoveRows();
	}
	saveFav();
}

bool DiskListModel::diskInFavExists(int i) {
	QString fileName = getDiskFileName(i);
	QString machine = getDiskMachine(i);
	if (machine.isEmpty())
		return false;

	int machineId = PathManager::instance()->machines().indexOf(machine);
	int favIndex = m_favList.indexOf(fileName);
	if (favIndex >= 0) {
		if (machineId == m_favListMachine.at(favIndex))
			return true;
	}
	return false;
}
