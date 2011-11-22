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
import "../base"

Page {
	id: galleryPage
	orientationLock: PageOrientation.LockPortrait

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-back"
			onClicked: appWindow.pageStack.pop()
		}
	}

	Menu {
		id: mainMenu
		property bool addRemoveIconToggle: false
		property bool addRemoveFavToggle: false
		property int diskIndex

		function prepareAndOpen(index) {
			mainMenu.diskIndex = index
			mainMenu.addRemoveIconToggle = diskListModel.iconInHomeScreenExists(mainMenu.diskIndex)
			mainMenu.addRemoveFavToggle = diskListModel.diskInFavExists(mainMenu.diskIndex)
			mainMenu.open()
		}

		MenuLayout {
			MenuItem {
				text: qsTr("Select Cover")
				onClicked: appWindow.pageStack.push(Qt.resolvedUrl("CoverSelectorPage.qml"),
													{ diskIndex: mainMenu.diskIndex })
			}
			MenuItem {
				text: qsTr("Add to Favourites")
				visible: !mainMenu.addRemoveFavToggle
				onClicked: diskListModel.addToFav(mainMenu.diskIndex)
			}
			MenuItem {
				text: qsTr("Remove from Favourites")
				visible: mainMenu.addRemoveFavToggle
				onClicked: diskListModel.removeFromFav(mainMenu.diskIndex)
			}
			MenuItem {
				text: qsTr("Create Icon in Home Screen")
				onClicked: galleryPage.homeScreenIcon(mainMenu.diskIndex)
				visible: !mainMenu.addRemoveIconToggle
			}
			MenuItem {
				text: qsTr("Remove Icon from Home Screen")
				onClicked: diskListModel.removeIconFromHomeScreen(mainMenu.diskIndex)
				visible: mainMenu.addRemoveIconToggle
			}
			MenuItem {
				text: qsTr("Remove Disk")
				onClicked: removeDiskDialog.prepareAndOpen(mainMenu.diskIndex)
			}
		}
	}

	ListModel { id: nullModel }
	function updateModel() {
		diskListView.model = nullModel
		diskListView.model = diskListModel
	}
	ListView {
		id: diskListView
		anchors.fill: parent
		spacing: 10
		delegate: Item {
			id: diskListViewDelegate
			width: parent.width
			height: 280

			Image {
				id: screenShot
				x: 10
				width: parent.width-20
				height: parent.height-30
				source: qsTr("image://disk/%1/%2*%3")
				.arg(diskListModel.getDiskMachine(index))
							.arg(title)
							.arg(screenShotUpdate)
			}
			Label {
				x: 10
				anchors.top: screenShot.bottom
				anchors.topMargin: 8
				text: title
				font.bold: true
				color: "white"
			}

			MouseArea {
				id: mouseArea
				anchors.fill: parent
				onClicked: diskGallery.launch(index, true)
				onPressAndHold: mainMenu.prepareAndOpen(index)
			}
			Behavior on scale {
				NumberAnimation { duration: 100 }
			}

			states: [
				State {
					name: "pressed"
					when: mouseArea.pressed
					PropertyChanges {
						target: diskListViewDelegate
						scale: 0.85
					}
				}
			]
		}
		section.property: "alphabet"
		section.criteria: ViewSection.FullString
		section.delegate: Item {
			width: parent.width
			height: 40
			Text {
				id: headerLabel
				width: 80
				height: parent.height
				anchors.right: parent.right
				text: section
				font.pointSize: 18
				color: theme.inverted ? "#4D4D4D" : "#3C3C3C";
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}
			Image {
				anchors.left: parent.left
				anchors.leftMargin: 10
				anchors.right: parent.right
				anchors.rightMargin: 80
				anchors.verticalCenter: headerLabel.verticalCenter
				source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
			}
		}
	}
	MySectionScroller {
		id: sectionScroller
		listView: diskListView
	}
	ScrollDecorator {
		flickableItem: diskListView
	}

	QueryDialog {
		id: removeDiskDialog
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		titleText: qsTr("Delete")
		onAccepted: diskListModel.trash(diskIndex)

		property int diskIndex

		function prepareAndOpen(index) {
			removeDiskDialog.diskIndex = index
			removeDiskDialog.message = qsTr("Do you really want to delete\n\"%1\" ?")
						.arg(diskListModel.getDiskTitle(index))
			removeDiskDialog.open()
		}
	}

	function homeScreenIcon(diskIndexArg) {
		if (diskListModel.getScreenShotUpdate(diskIndexArg) < 0) {
			errorDialog.message = qsTr("You need to make a screenshot first!")
			errorDialog.open()
		} else {
			var path = qsTr("image://disk/%1/%2*%3")
							.arg(diskListModel.getDiskMachine(diskIndexArg))
							.arg(diskListModel.getDiskTitle(diskIndexArg))
							.arg(diskListModel.getScreenShotUpdate(diskIndexArg))
			appWindow.pageStack.push(Qt.resolvedUrl("HomeScreenIconSheet.qml"),
									 { imgSource: path, diskIndex: diskIndexArg })
		}
	}
}
