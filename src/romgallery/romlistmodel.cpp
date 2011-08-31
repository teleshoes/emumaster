#include "romlistmodel.h"
#include "machineview.h"
#include <QDir>
#include <QFileInfo>

RomListModel::RomListModel(QObject *parent) :
	QAbstractListModel(parent),
	m_screenShotUpdateCounter(0) {
	QHash<int, QByteArray> roles;
	roles.insert(NameRole, "title");
	roles.insert(AlphabetRole, "alphabet");
	roles.insert(ScreenShotUpdate, "screenShotUpdate");
	setRoleNames(roles);
}

void RomListModel::setMachineName(const QString &name) {
	if (!m_list.isEmpty()) {
		beginRemoveRows(QModelIndex(), 0, m_list.size()-1);
		endRemoveRows();
	}
	m_machineName = name;
	emit machineNameChanged();
	QDir dir = QDir(QString("%1/%2")
					.arg(MachineView::romDirPath())
					.arg(name));
	m_list = dir.entryList(QDir::NoFilter, QDir::Name);
	m_list.removeOne(".");
	m_list.removeOne("..");
	for (int i = 0; i < m_list.size(); i++) {
		QFileInfo fileInfo(m_list.at(i));
		m_list[i] = fileInfo.completeBaseName();
	}
	if (!m_list.isEmpty()) {
		beginInsertRows(QModelIndex(), 0, m_list.size()-1);
		endInsertRows();
	}
}

QVariant RomListModel::data(const QModelIndex &index, int role) const {
	if (role == NameRole) {
		return m_list.at(index.row());
	} else if (role == AlphabetRole) {
		return m_list.at(index.row()).at(0).toUpper();
	} else if (role == ScreenShotUpdate) {
		return getScreenShotUpdate(index.row());
	}
	return QVariant();
}

int RomListModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent)
	return m_list.size();
}

int RomListModel::count() const
{ return m_list.size(); }

QString RomListModel::getAlphabet(int i) const
{ return m_list.at(i).at(0).toUpper(); }

QString RomListModel::get(int i) const
{ return m_list.at(i); }

void RomListModel::updateScreenShot(const QString &name) {
	int i = m_list.indexOf(name);
	if (i < 0)
		return;
	m_screenShotUpdateCounter++;
	emit dataChanged(index(i), index(i));
}

int RomListModel::getScreenShotUpdate(int i) const {
	QString path = QString("%1/screenshot/%2%3.jpg")
			.arg(MachineView::userDataDirPath())
			.arg(m_machineName)
			.arg(m_list.at(i));
	if (QFile::exists(path))
		return m_screenShotUpdateCounter;
	else
		return -1;
}
