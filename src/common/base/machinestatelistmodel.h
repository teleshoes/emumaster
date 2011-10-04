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

#ifndef MACHINESTATELISTMODEL_H
#define MACHINESTATELISTMODEL_H

class IMachine;
#include <QAbstractListModel>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

class MachineStateListModel : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
	static const int NewSlot		= -1;
	static const int AutoSlot		= -2;
	static const int InvalidSlot	= -3;

	enum RoleType {
		NameRole = Qt::UserRole+1,
		ScreenShotUpdate,
		DateTimeRole
	};
	explicit MachineStateListModel(IMachine *machine, const QString &diskName);
	int rowCount(const QModelIndex &parent) const;
	int count() const;
	QVariant data(const QModelIndex &index, int role) const;
	Q_INVOKABLE QString get(int i) const;
	QImage screenShot(int i) const;

	Q_INVOKABLE bool saveState(int i);
	Q_INVOKABLE bool loadState(int i);
	Q_INVOKABLE void removeState(int i);
signals:
	void countChanged();
private:
	int indexOf(int i) const;

	IMachine *m_machine;
	QDir m_dir;
	QFileInfoList m_list;
	int m_maxSaveIndex;
	int m_screenShotUpdateCounter;
};

#endif // MACHINESTATELISTMODEL_H
