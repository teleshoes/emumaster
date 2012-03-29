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
import com.nokia.meego 1.1

Page {
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

	Flickable {
		id: flickable
		anchors.fill: parent
		contentWidth: width
		contentHeight: collectionGrid.height
		visible: !diskGallery.massStorageInUse

		Grid {
			id: collectionGrid
			rows: appWindow.inPortrait ? 4 : 3
			columns: appWindow.inPortrait ? 2 : 3
			anchors.horizontalCenter: parent.horizontalCenter

			CollectionTypeButton { name: "fav" }
			CollectionTypeButton { name: "nes" }
			CollectionTypeButton { name: "snes" }
			CollectionTypeButton { name: "gba" }
			CollectionTypeButton { name: "psx" }
			CollectionTypeButton { name: "amiga" }
			CollectionTypeButton { name: "pico" }
		}

	}
	ScrollDecorator { flickableItem: flickable }

	MassStorageWarning {}
}
