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

#include "diskgallery.h"
#include <base/configuration.h>
#include <base/pathmanager.h>
#include <QApplication>
#include <QSystemInfo>
#include <QDeclarativeEngine>

QTM_USE_NAMESPACE

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QSystemInfo sysInfo;
	if (sysInfo.version(QSystemInfo::Os) < QString("1.2")) {
		QDeclarativeView view;
		QObject::connect(view.engine(), SIGNAL(quit()), &view, SLOT(close()));
		QString qmlPath = QString("%1/qml/gallery/osVersionError.qml")
				.arg(pathManager.installationDirPath());
		view.setSource(QUrl::fromLocalFile(qmlPath));
		view.showFullScreen();
		return app.exec();
	}

	Configuration::setupAppInfo();
	pathManager.buildLocalDirTree();

	DiskGallery view;
	view.showFullScreen();
	return app.exec();
}
