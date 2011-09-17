import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	id: romChooserPage
	property int currentRomIndex: -1
	property bool coverFlowEnabled: true
	property bool isDiskVisibleAndSelected: currentRomIndex >= 0 && tabGroup.currentTab !== machineTypeTab

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-mediacontrol-play"
			visible: isDiskVisibleAndSelected
			onClicked: romGallery.launch(romListModel.get(currentRomIndex), true)
		}
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
			iconId: "toolbar-view-menu"
			onClicked: {
				mainMenu.addRemoveIconToggle = romGallery.iconInHomeScreenExists(romListModel.get(currentRomIndex))
				mainMenu.open()
			}
		}
	}

	Menu {
		id: mainMenu
		property bool addRemoveIconToggle: false

		MenuLayout {
			MenuItem {
				text: qsTr("Select Cover")
				onClicked: {
					coverSelectorSheet.open()
					coverSelectorSheet.displayHelp()
				}
				visible: romChooserPage.isDiskVisibleAndSelected
			}
			MenuItem {
				text: qsTr("Create Icon in Home Screen")
				onClicked: romChooserPage.homeScreenIcon()
				visible: romChooserPage.isDiskVisibleAndSelected && !mainMenu.addRemoveIconToggle
			}
			MenuItem {
				text: qsTr("Remove Icon from Home Screen")
				onClicked: romGallery.removeIconFromHomeScreen(romListModel.get(currentRomIndex))
				visible: romChooserPage.isDiskVisibleAndSelected && mainMenu.addRemoveIconToggle
			}
			MenuItem {
				text: qsTr("Remove Disk")
				onClicked: removeRomDialog.open()
				visible: romChooserPage.isDiskVisibleAndSelected
			}
			MenuItem {
				text: qsTr("Run With Autoload Disabled")
				onClicked: romGallery.launch(romListModel.get(currentRomIndex), false)
				visible: romChooserPage.isDiskVisibleAndSelected
			}
			MenuItem {
				text: qsTr("About EmuMaster ...")
				onClicked: aboutSheet.open()
			}
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

	CoverSelectorSheet {
		id: coverSelectorSheet
		onAccepted: romListModel.setDiskCover(currentRomIndex, selectedPath)
	}

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

	function homeScreenIcon() {
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
