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
	id: diskChooserPage
	property int currentDiskIndex: -1
	property bool coverFlowEnabled: true
	property bool isDiskVisibleAndSelected: currentDiskIndex >= 0 && tabGroup.currentTab !== machineTypeTab

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-mediacontrol-play"
			visible: isDiskVisibleAndSelected
			onClicked: diskGallery.launch(currentDiskIndex, true)
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
				mainMenu.addRemoveIconToggle = diskListModel.iconInHomeScreenExists(currentDiskIndex)
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
				visible: diskChooserPage.isDiskVisibleAndSelected
			}
			MenuItem {
				text: qsTr("Create Icon in Home Screen")
				onClicked: diskChooserPage.homeScreenIcon()
				visible: diskChooserPage.isDiskVisibleAndSelected && !mainMenu.addRemoveIconToggle
			}
			MenuItem {
				text: qsTr("Remove Icon from Home Screen")
				onClicked: diskListModel.removeIconFromHomeScreen(currentDiskIndex)
				visible: diskChooserPage.isDiskVisibleAndSelected && mainMenu.addRemoveIconToggle
			}
			MenuItem {
				text: qsTr("Remove Disk")
				onClicked: removeDiskDialog.open()
				visible: diskChooserPage.isDiskVisibleAndSelected
			}
			MenuItem {
				text: qsTr("Run With Autoload Disabled")
				onClicked: diskGallery.launch(currentDiskIndex, false)
				visible: diskChooserPage.isDiskVisibleAndSelected
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
		onAccepted: diskListModel.setDiskCover(currentDiskIndex, selectedPath)
	}

	QueryDialog {
		id: errorDialog
		message: ""
		rejectButtonText: qsTr("Close")
	}

	QueryDialog {
		id: howToInstallDiskDialog
		rejectButtonText: qsTr("Close")
		titleText: qsTr("Help")
		message: qsTr("The directory is empty. To install a disk you need to attach " +
					  "the phone to the PC and copy your files to \"emumaster/%1\" " +
					  "directory. Remember to detach the phone from the PC.")
						.arg(diskListModel.collection)
	}

	QueryDialog {
		id: firstRunMsg
		titleText: qsTr("Info")
		rejectButtonText: qsTr("Close")
		message:	qsTr("Hi! Few clues: Please let me know about any problems " +
						"before you make a review in the Store since I cannot " +
						"answer you there. You can find contact info at the about " +
						"page:\n Menu->About EmuMaster...\n Enjoy :)\n " +
						"Consider a donation if you find this software useful.") +
						(diskGallery.runCount < 10
							?	qsTr("\n\nThis message will be shown %1 more times")
									.arg(10-diskGallery.runCount)
							: "")

	}

	QueryDialog {
		id: removeDiskDialog
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		titleText: qsTr("Delete")
		message: qsTr("Do you really want to delete \"%1\" ?")
					.arg(diskListModel.getDiskTitle(currentDiskIndex))

		onAccepted: diskListModel.trash(currentDiskIndex)
	}

	QueryDialog {
		id: detachUsbDialog
		message: qsTr(	"\"emumaster\" folder not found! Detach USB cable if " +
						"connected and restart the application."	)
		rejectButtonText: qsTr("Close")
		onRejected: Qt.quit()
	}

	Connections {
		target: diskGallery
		onDiskUpdate: selectCollection(diskListModel.collectionLastUsed)
		onDetachUsb: detachUsbDialog.open()
		onShowFirstRunMsg: firstRunMsg.open()
	}
	AboutSheet { id: aboutSheet }

	function selectCollection(name) {
		diskListModel.collection = name
		if (diskListModel.count > 0) {
			currentDiskIndex = 0
		} else {
			howToInstallDiskDialog.open()
		}
		listTab.update()
	}

	function homeScreenIcon() {
		if (diskListModel.getScreenShotUpdate(currentDiskIndex) < 0) {
			errorDialog.message = qsTr("You need to make a screenshot first!")
			errorDialog.open()
		} else {
			saveIconSheet.imgScale = 1.0
			saveIconSheet.iconX = 0
			saveIconSheet.iconY = 0
			saveIconSheet.imgSource = qsTr("image://disk/%1/%2*%3")
										.arg(diskListModel.collection)
										.arg(diskListModel.getDiskTitle(currentDiskIndex))
										.arg(diskListModel.getScreenShotUpdate(currentDiskIndex))
			saveIconSheet.open()
		}
	}
}
