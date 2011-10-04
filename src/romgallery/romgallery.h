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

class RomListModel;
#include <QDeclarativeView>

class RomGallery : public QDeclarativeView {
    Q_OBJECT
public:
    explicit RomGallery(QWidget *parent = 0);
	~RomGallery();

	Q_INVOKABLE void launch(int index, bool autoload);

	Q_INVOKABLE bool addIconToHomeScreen(int index, qreal scale, int x, int y);
	Q_INVOKABLE void removeIconFromHomeScreen(int index);
	Q_INVOKABLE bool iconInHomeScreenExists(int index);

	Q_INVOKABLE void donate();
	Q_INVOKABLE void homepage();
signals:
	void romUpdate();
	void detachUsb();
private slots:
	void emitRomUpdate();
private:
	QImage applyMaskAndOverlay(const QImage &icon);
	void homeScreenIconPaths(QString *desktopFilePath, QString *iconFilePath, const QString &diskTitle);

	RomListModel *m_romListModel;
};

#endif // ROMGALLERY_H
