/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef ROMLISTMODEL_H
#define ROMLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QDir>

class DiskListModel : public QAbstractListModel {
    Q_OBJECT
	Q_PROPERTY(QString collection READ collection WRITE setCollection NOTIFY collectionChanged)
	Q_PROPERTY(int count READ count NOTIFY collectionChanged)
	Q_PROPERTY(QString collectionLastUsed READ collectionLastUsed CONSTANT)
public:
	enum RoleType {
		TitleRole = Qt::UserRole+1,
		MachineRole,
		AlphabetRole,
		ScreenShotUpdate
	};
	explicit DiskListModel(QObject *parent = 0);
	~DiskListModel();

	QString collection() const;
	void setCollection(const QString &name);
	QString collectionLastUsed() const;

	int rowCount(const QModelIndex &parent) const;
	int count() const;

	QVariant data(const QModelIndex &index, int role) const;
	void updateScreenShot(const QString &name);

	Q_INVOKABLE QString getDiskFileName(int i) const;
	Q_INVOKABLE QString getDiskTitle(int i) const;
	Q_INVOKABLE QString getDiskMachine(int i) const;
	Q_INVOKABLE QString getAlphabet(int i) const;
	Q_INVOKABLE int getScreenShotUpdate(int i) const;
	Q_INVOKABLE void trash(int i);
	Q_INVOKABLE void setDiskCover(int i, const QUrl &coverUrl);

	Q_INVOKABLE void addToFav(int i);
	Q_INVOKABLE void removeFromFav(int i);
	Q_INVOKABLE bool diskInFavExists(int i);

	Q_INVOKABLE bool addIconToHomeScreen(int index, qreal scale, int x, int y);
	Q_INVOKABLE void removeIconFromHomeScreen(int index);
	Q_INVOKABLE bool iconInHomeScreenExists(int index);
signals:
	void collectionChanged();
private:
	void setCollectionMachine();
	void setCollectionFav();

	void setupFilters();
	void setupNesFilter();
	void setupGbaFilter();
	void setupSnesFilter();
	void setupPsxFilter();
	void setupAmigaFilter();

	void loadFav();
	void saveFav();

	QImage applyMaskAndOverlay(const QImage &icon);
	bool createDesktopFile(const QString &fileName,
						   const QString &title,
						   const QString &exec,
						   const QString &icon,
						   const QString &categories);

	QString m_collection;
	QString m_collectionLastUsed;

	QStringList m_list;
	QList<int> m_listMachine;

	int m_screenShotUpdateCounter;

	class DiskFilter {
	public:
		QList<QRegExp> included;
		QList<QRegExp> excluded;
	};
	QMap<QString, DiskFilter> m_diskFilters;

	QStringList m_favList;
	QList<int> m_favListMachine;
};

inline QString DiskListModel::collection() const
{ return m_collection; }
inline QString DiskListModel::collectionLastUsed() const
{ return m_collectionLastUsed; }

#endif // ROMLISTMODEL_H
