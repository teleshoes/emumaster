/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "statelistmodel.h"
#include "imachine.h"
#include "pathmanager.h"
#include <QDataStream>
#include <QDateTime>
#include <QImage>

StateListModel::StateListModel(IMachine *machine, const QString &diskFileName) :
	m_machine(machine),
	m_screenShotUpdateCounter(0) {

	QHash<int, QByteArray> roles;
	roles.insert(NameRole, "title");
	roles.insert(ScreenShotUpdate, "screenShotUpdate");
	roles.insert(DateTimeRole, "saveDateTime");
	setRoleNames(roles);

	QString diskTitle = QFileInfo(diskFileName).completeBaseName();
	QString path = PathManager::instance()->stateDirPath(diskTitle);
	m_dir.mkpath(path);
	m_dir = QDir(path);

	m_list = m_dir.entryInfoList(QDir::Files, QDir::Time);
	m_maxSaveIndex = 0;
	foreach (QFileInfo info, m_list) {
		int i = info.fileName().toInt();
		m_maxSaveIndex = qMax(i, m_maxSaveIndex);
	}
}

QVariant StateListModel::data(const QModelIndex &index, int role) const {
	if (role == NameRole) {
		return m_list.at(index.row()).fileName();
	} else if (role == ScreenShotUpdate) {
		return m_screenShotUpdateCounter;
	} else if (role == DateTimeRole) {
		return m_list.at(index.row()).lastModified();
	}
	return QVariant();
}

int StateListModel::rowCount(const QModelIndex &parent) const {
	Q_UNUSED(parent)
	return m_list.size();
}

int StateListModel::count() const
{ return m_list.size(); }

QString StateListModel::indexToSlot(int i) const
{ return m_list.at(i).fileName(); }

QImage StateListModel::screenShot(int slot) const {
	QString diskPath = m_dir.filePath(QString::number(slot));
	QFile file(diskPath);
	if (!file.open(QIODevice::ReadOnly))
		return QImage();

	QDataStream s(&file);
	s.setByteOrder(QDataStream::LittleEndian);
	s.setFloatingPointPrecision(QDataStream::SinglePrecision);
	QImage screenShot;
	s >> screenShot;
	return screenShot;
}

bool StateListModel::saveState(int slot) {
	if (!m_machine)
		return false;
	bool newState = false;
	if (slot == NewSlot) {
		slot = ++m_maxSaveIndex;
		newState = true;
	} else if (slot == AutoSaveLoadSlot) {
		newState = (indexOfSlot(AutoSaveLoadSlot) < 0);
	}

	QString statePath = m_dir.filePath(QString::number(slot));
	if (!m_machine->saveState(statePath)) {
		emit slFailed();
		return false;
	}

	m_screenShotUpdateCounter++;
	if (newState) {
		beginInsertRows(QModelIndex(), 0, 0);
		m_list.prepend(QFileInfo(statePath));
		endInsertRows();
		emit countChanged();
	} else {
		int i = indexOfSlot(slot);
		if (i >= 0) {
			m_list[i] = QFileInfo(m_list.at(i).filePath());
			emit dataChanged(index(i), index(i));
		}
	}
	return true;
}

bool StateListModel::loadState(int slot) {
	Q_ASSERT(m_machine != 0);

	QString statePath = m_dir.filePath(QString::number(slot));
	bool ok = m_machine->loadState(statePath);
	if (!ok)
		emit slFailed();
	else
		emit stateLoaded();
	return ok;
}

void StateListModel::removeState(int slot) {
	int i = indexOfSlot(slot);
	if (i >= 0) {
		beginRemoveRows(QModelIndex(), i, i);
		m_dir.remove(QString::number(slot));
		m_list.removeAt(i);
		endRemoveRows();
	}
}

void StateListModel::removeAll() {
	while (!m_list.isEmpty())
		removeState(m_list.at(0).fileName().toInt());
}

int StateListModel::indexOfSlot(int slot) const {
	for (int x = 0; x < m_list.size(); x++) {
		if (m_list.at(x).fileName().toInt() == slot)
			return x;
	}
	return -1;
}

bool StateListModel::exists(int slot) const {
	return m_dir.exists(QString::number(slot));
}
