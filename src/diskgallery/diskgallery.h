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

#ifndef ROMGALLERY_H
#define ROMGALLERY_H

class DiskListModel;
#include <QDeclarativeView>
#include <QUdpSocket>
#include <QSettings>

class DiskGallery : public QDeclarativeView {
	Q_OBJECT
	Q_PROPERTY(int runCount READ runCount CONSTANT)
public:
	explicit DiskGallery(QWidget *parent = 0);
	~DiskGallery();

	int runCount() const;

	Q_INVOKABLE void launch(int index);
	Q_INVOKABLE void advancedLaunch(int index, bool autoSaveLoad, const QString &confStr);

	Q_INVOKABLE void donate();
	Q_INVOKABLE void homepage();
	Q_INVOKABLE void wiki();
	Q_INVOKABLE void sixAxisMonitor();

	Q_INVOKABLE QVariant globalOption(const QString &name);
	Q_INVOKABLE void setGlobalOption(const QString &name, const QVariant &value);
signals:
	void diskUpdate();
	void detachUsb();
	void showFirstRunMsg();
private slots:
	void emitDiskUpdate();
	void receiveDatagram();
private:
	void setupQml();
	void incrementRunCount();

	DiskListModel *m_diskListModel;
	int m_runCount;
	QUdpSocket m_sock;
	QSettings m_settings;
};

#endif // ROMGALLERY_H
