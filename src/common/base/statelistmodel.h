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

#ifndef STATELISTMODEL_H
#define STATELISTMODEL_H

class IMachine;
#include <QAbstractListModel>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

class StateListModel : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
	static const int NewSlot				= -1;
	static const int AutoSaveLoadSlot		= -2;
	static const int InvalidSlot			= -3;

	enum RoleType {
		NameRole = Qt::UserRole+1,
		ScreenShotUpdate,
		DateTimeRole
	};

	explicit StateListModel(IMachine *machine, const QString &diskFileName);
	int rowCount(const QModelIndex &parent) const;
	int count() const;
	QVariant data(const QModelIndex &index, int role) const;
	QImage screenShot(int slot) const;

	Q_INVOKABLE QString indexToSlot(int i) const;
	Q_INVOKABLE bool saveState(int slot);
	Q_INVOKABLE bool loadState(int slot);
	Q_INVOKABLE void removeState(int slot);
	Q_INVOKABLE void removeAll();
signals:
	void countChanged();
	void slFailed();
private:
	int indexOfSlot(int i) const;

	IMachine *m_machine;
	QDir m_dir;
	QFileInfoList m_list;
	int m_maxSaveIndex;
	int m_screenShotUpdateCounter;
};

#endif // STATELISTMODEL_H