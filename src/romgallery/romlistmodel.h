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
#include <QUdpSocket>

class RomListModel : public QAbstractListModel {
    Q_OBJECT
	Q_PROPERTY(QString machineName READ machineName WRITE setMachineName NOTIFY machineNameChanged)
	Q_PROPERTY(int count READ count NOTIFY machineNameChanged)
	Q_PROPERTY(QString machineNameLastUsed READ machineNameLastUsed CONSTANT)
public:
	enum RoleType {
		TitleRole = Qt::UserRole+1,
		AlphabetRole,
		ScreenShotUpdate
	};
    explicit RomListModel(QObject *parent = 0);
	~RomListModel();
	QString machineName() const;
	void setMachineName(const QString &name);
	QString machineNameLastUsed() const;
	int rowCount(const QModelIndex &parent) const;
	int count() const;
	QVariant data(const QModelIndex &index, int role) const;
	void updateScreenShot(const QString &name);

	Q_INVOKABLE QString getDiskFileName(int i) const;
	Q_INVOKABLE QString getDiskTitle(int i) const;
	Q_INVOKABLE QString getAlphabet(int i) const;
	Q_INVOKABLE int getScreenShotUpdate(int i) const;
	Q_INVOKABLE void trash(int i);
	Q_INVOKABLE void setDiskCover(int i, const QUrl &coverUrl);
signals:
	void machineNameChanged();
private slots:
	void receiveDatagram();
private:
	int m_screenShotUpdateCounter;
	QStringList m_list;
	QString m_machineName;
	QDir m_dir;
	QString m_machineNameLastUsed;
	QUdpSocket m_sock;
};

inline QString RomListModel::machineName() const
{ return m_machineName; }
inline QString RomListModel::machineNameLastUsed() const
{ return m_machineNameLastUsed; }

#endif // ROMLISTMODEL_H
