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

import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	orientationLock: PageOrientation.LockPortrait

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-view-menu"
			anchors.right: parent.right
			onClicked: mainMenu.open()
		}
	}

	Menu {
		id: mainMenu

		MenuLayout {
			MenuItem {
				text: qsTr("Global Settings")
				onClicked: appWindow.pageStack.push(Qt.resolvedUrl("GlobalSettings.qml"))
			}
			MenuItem {
				text: qsTr("SixAxis Monitor")
				onClicked: diskGallery.sixAxisMonitor()
			}
			MenuItem {
				text: qsTr("About EmuMaster ...")
				onClicked: appWindow.pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
			}
		}
	}

	Grid {
		anchors.centerIn: parent
		rows: 3
		columns: 2

		CollectionTypeButton { name: "nes" }
		CollectionTypeButton { name: "snes" }
		CollectionTypeButton { name: "gba" }
		CollectionTypeButton { name: "psx"; alphaVersion: true }
		CollectionTypeButton { name: "amiga" }
		CollectionTypeButton { name: "fav" }
	}
}
