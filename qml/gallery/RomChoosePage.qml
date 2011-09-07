import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	id: romChooserPage
	property int currentRomIndex: -1
	property bool coverFlowEnabled: true

	tools: ToolBarLayout {
		ButtonRow {
			platformStyle: TabButtonStyle { }
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-dialer"
				tab: machineTypeTab
			}
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-gallery"
				tab: galleryTab
			}
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-list"
				tab: listTab
			}
		}
		ToolIcon {
			iconId: "toolbar-contact"
			onClicked: aboutSheet.open()
		}
		ToolIcon {
			iconId: "toolbar-delete"
			visible: currentRomIndex >= 0
			onClicked: removeRomDialog.open()
		}
		ToolIcon {
			iconId: "toolbar-home"
			visible: currentRomIndex >= 0
			onClicked: {
				if (romListModel.getScreenShotUpdate(currentRomIndex) < 0) {
					errorDialog.message = "You need to make screenshot first!"
					errorDialog.open()
				} else {
					saveIconSheet.imgScale = 1.0
					saveIconSheet.iconX = 0
					saveIconSheet.iconY = 0
					saveIconSheet.imgSource = "image://rom/" + romListModel.machineName + "_" + romListModel.get(currentRomIndex) + "*" + romListModel.getScreenShotUpdate(currentRomIndex)
					saveIconSheet.open()
				}
			}
		}
		ToolIcon {
			iconId: "toolbar-mediacontrol-play"
			visible: currentRomIndex >= 0
			onClicked: romGallery.launch(romListModel.get(currentRomIndex))
		}
	}

	TabGroup {
		id: tabGroup
		currentTab: machineTypeTab

		MachineTypePage { id: machineTypeTab }
		GalleryPage { id: galleryTab }
		ListPage { id: listTab }

		onCurrentTabChanged: coverFlowEnabled = (currentTab === galleryTab)
	}

	HomeScreenIconSheet { id: saveIconSheet }

	QueryDialog {
		id: errorDialog
		icon: "image://theme/icon-m-common-red"
		message: ""
		rejectButtonText: "Close"
	}

	QueryDialog {
		id: howToInstallRomDialog

		rejectButtonText: "Close"

		titleText: "Help"
		message: "ROMs not found. To install ROM you need to attach phone to the PC and copy your files to \"emumaster/" +
				 romListModel.machineName + "\" directory. Remember to detach phone from the PC. " +
				 "Consider a small donation if you find this software useful"
	}

	QueryDialog {
		id: removeRomDialog

		acceptButtonText: "Yes"
		rejectButtonText: "No"

		titleText: "Remove"
		message: "Do you really want to remove \"" + romListModel.get(currentRomIndex) + "\" ?"

		onAccepted: romListModel.trash(currentRomIndex)
	}

	QueryDialog {
		id: detachUsbDialog
		icon: "image://theme/icon-m-common-red"
		message: "\"emumaster\" folder not found! Detach USB cable if connected and restart application."
		rejectButtonText: "Close"
		onRejected: Qt.quit()
	}

	Connections {
		target: romGallery
		onRomUpdate: setMachineName(romListModel.machineNameLastUsed)
		onDetachUsb: detachUsbDialog.open()
	}
	AboutSheet { id: aboutSheet }

	function setMachineName(name) {
		romListModel.machineName = name
		if (romListModel.count > 0) {
			currentRomIndex = 0
		} else {
			howToInstallRomDialog.open()
		}
		listTab.update()
	}
}
