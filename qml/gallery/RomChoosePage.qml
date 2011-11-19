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
	id: romChooserPage
	property int currentRomIndex: -1
	property bool coverFlowEnabled: true
	property bool isDiskVisibleAndSelected: currentRomIndex >= 0 && tabGroup.currentTab !== machineTypeTab

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-mediacontrol-play"
			visible: isDiskVisibleAndSelected
			onClicked: romGallery.launch(currentRomIndex, true)
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
				mainMenu.addRemoveIconToggle = romGallery.iconInHomeScreenExists(currentRomIndex)
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
				onClicked: romGallery.removeIconFromHomeScreen(currentRomIndex)
				visible: romChooserPage.isDiskVisibleAndSelected && mainMenu.addRemoveIconToggle
			}
			MenuItem {
				text: qsTr("Remove Disk")
				onClicked: removeRomDialog.open()
				visible: romChooserPage.isDiskVisibleAndSelected
			}
			MenuItem {
				text: qsTr("Run With Autoload Disabled")
				onClicked: romGallery.launch(currentRomIndex, false)
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
		message: ""
		rejectButtonText: "Close"
	}

	QueryDialog {
		id: howToInstallRomDialog

		rejectButtonText: "Close"

		titleText: "Help"
		message: "The directory is empty. To install disk you need to attach phone to the PC and copy your files to \"emumaster/" +
				 romListModel.machineName + "\" directory. Remember to detach phone from the PC. " +
				 "Consider a small donation if you find this software useful"
	}

	QueryDialog {
		id: removeRomDialog

		acceptButtonText: "Yes"
		rejectButtonText: "No"

		titleText: "Remove"
		message: "Do you really want to remove \"" + romListModel.getDiskTitle(currentRomIndex) + "\" ?"

		onAccepted: romListModel.trash(currentRomIndex)
	}

	QueryDialog {
		id: detachUsbDialog
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
			saveIconSheet.imgSource = "image://rom/" + romListModel.machineName + "_" + romListModel.getDiskTitle(currentRomIndex) + "*" + romListModel.getScreenShotUpdate(currentRomIndex)
			saveIconSheet.open()
		}
	}
}
