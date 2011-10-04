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

#include "romgallery.h"
#include "imachine.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	IMachine::buildLocalDirTree();

	RomGallery view;
#if defined(MEEGO_EDITION_HARMATTAN)
	view.showFullScreen();
#else
	view.resize(854, 480);
	view.show();
#endif
	return app.exec();
}
