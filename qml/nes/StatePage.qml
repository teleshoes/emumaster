import QtQuick 1.1
import com.nokia.meego 1.0
import "../common"
import "../common/utils.js" as EmuMasterUtils

Page {
	CoverFlow {
		id: coverFlow
		anchors.fill: parent
		model: stateListModel
		delegate: CoverFlowDelegate {
			imageSource: "image://machine/" + title + "*nes" + screenShotUpdate

			Label {
				anchors.top: parent.bottom
				anchors.topMargin: 30
				anchors.horizontalCenter: parent.horizontalCenter
				text: Qt.formatDateTime(saveDateTime, "dd.MM.yyyy hh:mm:ss")
				visible: coverFlow.currentIndex == index
				color: "red"
			}
			Label {
				anchors.top: parent.bottom
				anchors.topMargin: 50
				anchors.horizontalCenter: parent.horizontalCenter
				text: "autosave"
				visible: coverFlow.currentIndex == index && title == -2
				color: "blue"
			}
		}
		pathItemCount: 7

		Component.onCompleted: {
			if (stateListModel.count > 0)
				coverFlow.currentIndex = 0
			else
				coverFlow.currentIndex = -1
		}
	}
	ButtonRow {
		anchors.horizontalCenter: parent.horizontalCenter
		exclusive: false

		Button {
			text: "Save"
			onClicked: {
				if (!stateListModel.saveState(-1))
					errorDialog.open()
			}
		}
		Button {
			text: "Overwrite"
			enabled: coverFlow.currentIndex >= 0
			onClicked: overwriteDialog.open()
		}
		Button {
			text: "Reset"
			onClicked: machine.reset()
		}
		Button {
			text: "Remove"
			enabled: coverFlow.currentIndex >= 0
			onClicked: removeDialog.open()
		}
		Button {
			text: "Load"
			enabled: coverFlow.currentIndex >= 0
			onClicked: {
				if (!stateListModel.loadState(stateListModel.get(coverFlow.currentIndex)))
					errorDialog.open()
			}
		}
	}

	QueryDialog {
		id: overwriteDialog
		titleText: "Really?"
		message: "Do you really want to overwrite the state?"
		acceptButtonText: "Yes"
		rejectButtonText: "No"
		onAccepted: {
			if (!stateListModel.saveState(stateListModel.get(coverFlow.currentIndex)))
				errorDialog.open()
		}
	}


	QueryDialog {
		id: removeDialog
		titleText: "Really?"
		message: "Do you really want to remove the state?"
		acceptButtonText: "Yes"
		rejectButtonText: "No"
		onAccepted: {
			stateListModel.removeState(stateListModel.get(coverFlow.currentIndex))
		}
	}

	QueryDialog {
		id: errorDialog
		icon: "image://theme/icon-m-common-red"
		titleText: "Oops"
		message: "Something went wrong!"
		acceptButtonText: "Close"
		rejectButtonText: ""
	}
	Button {
		anchors.right: parent.right
		anchors.rightMargin: 10
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 10
		iconSource: "image://theme/icon-s-music-video-description"
		onClicked: infoSheet.open()
	}
	Sheet {
		id: infoSheet

		acceptButtonText: ""
		rejectButtonText: "Close"

		title: Label {
			anchors.centerIn: parent
			text: "Machine Info"
		}
		content: Flickable {
			anchors.fill: parent
			flickableDirection: Flickable.VerticalFlick
			Column {
				width: parent.width

				Label {
					width: parent.width
					text: "Disk ROM size: " + EmuMasterUtils.byteCountToText(machine.disk.romSize)
					wrapMode: Text.WordWrap
					horizontalAlignment: Text.AlignHCenter
				}
				Label {
					width: parent.width
					text: "Disk video ROM size: " + EmuMasterUtils.byteCountToText(machine.disk.vromSize)
					wrapMode: Text.WordWrap
					horizontalAlignment: Text.AlignHCenter
				}
				Label {
					width: parent.width
					text: "Disk RAM size: " + EmuMasterUtils.byteCountToText(machine.disk.romSize)
					wrapMode: Text.WordWrap
					horizontalAlignment: Text.AlignHCenter
				}
				Label {
					width: parent.width
					text: "Disk CRC: " + EmuMasterUtils.byteCountToText(machine.disk.crc)
					wrapMode: Text.WordWrap
					horizontalAlignment: Text.AlignHCenter
				}
				Label {
					width: parent.width
					text: "Mapper: " + machine.mapper.name + " (" + machine.disk.machineType + ")"
					wrapMode: Text.WordWrap
					horizontalAlignment: Text.AlignHCenter
				}
			}
		}
	}
}
