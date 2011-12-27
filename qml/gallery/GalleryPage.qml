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

	GalleryMenu { id: galleryMenu }

	ListModel { id: nullModel }

	function updateModel() {
		diskListView.model = nullModel
		diskListView.model = diskListModel

		diskListViewLandscape.model = nullModel
		diskListViewLandscape.model = diskListModel
	}
	ListView {
		id: diskListView
		anchors.fill: parent
		spacing: 10
		visible: appWindow.inPortrait

		delegate: ImageListViewDelegate {
			imgSource: qsTr("image://disk/%1/%2*%3")
							.arg(diskListModel.getDiskMachine(index))
							.arg(title)
							.arg(screenShotUpdate)
			text: title
			width: 480
			height: 280
			onClicked: diskGallery.launch(index, true)
			onPressAndHold: galleryMenu.prepareAndOpen(index)
		}
		section.property: "alphabet"
		section.criteria: ViewSection.FullString
		section.delegate: SectionSeperator { text: section }
	}
	MySectionScroller { listView: diskListView }
	ScrollDecorator {
		flickableItem: diskListView
		__minIndicatorSize: 80
	}

	GridView {
		id: diskListViewLandscape
		anchors.fill: parent
		visible: !appWindow.inPortrait
		cellWidth: 284
		cellHeight: 240

		delegate: ImageListViewDelegate {
			imgSource: qsTr("image://disk/%1/%2*%3")
							.arg(diskListModel.getDiskMachine(index))
							.arg(title)
							.arg(screenShotUpdate)
			text: titleElided
			width: 270
			height: 220
			onClicked: diskGallery.launch(index, true)
			onPressAndHold: galleryMenu.prepareAndOpen(index)
		}
	}
	ScrollDecorator {
		flickableItem: diskListViewLandscape
		__minIndicatorSize: 80
	}

	tools: ToolBarLayout {
		id: galleryToolBar

		property bool searchVisible: false

		ToolIcon {
			iconId: "toolbar-back"
			onClicked: {
				appWindow.pageStack.pop()
				galleryToolBar.searchVisible = false
			}
		}
		TextField {
			id: searchField
			anchors.verticalCenter: parent.verticalCenter
			placeholderText: qsTr("Search")
			visible: galleryToolBar.searchVisible
			inputMethodHints: Qt.ImhNoPredictiveText|Qt.ImhNoAutoUppercase

			onTextChanged: {
				if (visible)
					diskListModel.setNameFilter(searchField.text)
			}
			onVisibleChanged: {
				if (!visible)
					text = ""
			}
		}

		ToolIcon {
			iconId: "icon-m-common-search-inverse"
			anchors.right: parent.right
			onClicked: galleryToolBar.searchVisible = !galleryToolBar.searchVisible
		}
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
