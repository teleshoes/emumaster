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

Sheet {
	id: coverSelector
	property alias folder: folderModel.folder
	property string selectedPath

	acceptButtonText: "OK"
	// TODO waiting for new version of components - accept enable disable
	rejectButtonText: "Cancel"

	title: Label {
		anchors.centerIn: parent
		text: "Select Cover"
	}

	FolderListModel {
		id: folderModel
		folder: "file:/home/user/MyDocs/emumaster/covers"
		nameFilters: ["*.jpg","*.png"]
	}
	content: CoverFlow {
		id: coverFlow
		anchors.fill: parent
		model: folderModel
		delegate: CoverFlowDelegate {
			id: coverFlowDelegate
			property string filePath: coverSelector.folder + "/" + fileName
			property bool selected: coverFlow.currentIndex === index
			imageSource: filePath

			states: [
				State {
					name: "current"
					when: coverFlowDelegate.selected
					PropertyChanges {
						target: coverSelector
						restoreEntryValues: false
						selectedPath: coverFlowDelegate.filePath
					}
				}
			]
		}
		pathItemCount: 7
	}
	QueryDialog {
		id: whereToCopyCoversDialog

		rejectButtonText: "Close"

		titleText: "Help"
		message: "Copy your covers (.jpg and .png files) to \"emumaster/covers\" directory"
		onRejected: coverSelector.reject()
	}
	function displayHelp() {
		if (folderModel.count <= 0)
			whereToCopyCoversDialog.open()
	}
}
