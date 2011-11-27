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
import Qt.labs.folderlistmodel 1.0
import "../base"

Page {
	id: coverSelector
	orientationLock: PageOrientation.LockPortrait

	property int diskIndex
	property alias folder: folderModel.folder
	property string selectedPath

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-back"
			onClicked: appWindow.pageStack.pop()
		}
	}

	Label {
		id: helpLabel
		anchors.centerIn: parent
		width: parent.width
		text: qsTr("Copy your covers (.jpg files) to\n\"emumaster/covers\"")
		font.bold: true
		horizontalAlignment: Text.AlignHCenter
		visible: false
	}

	FolderListModel {
		id: folderModel
		folder: "file:/home/user/MyDocs/emumaster/covers"
		nameFilters: ["*.jpg"]
		showDirs: false
	}
	ListView {
		id: coverView
		anchors.fill: parent
		model: folderModel
		spacing: 10
		delegate: ImageListViewDelegate {
			property string filePath: coverSelector.folder + "/" + fileName
			imgSource: filePath
			onClicked: {
				appWindow.pageStack.pop()
				diskListModel.setDiskCover(coverSelector.diskIndex,
										   coverViewDelegate.filePath)
			}
		}
	}
	Component.onCompleted: {
		if (folderModel.count <= 0)
			helpLabel.visible = true
	}
}
