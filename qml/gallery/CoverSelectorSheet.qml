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
