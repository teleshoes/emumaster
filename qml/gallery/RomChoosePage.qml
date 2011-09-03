import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	id: romChooserPage
	property int currentRomIndex: -1
	property bool coverFlowEnabled: true

	tools: ToolBarLayout {
//	TODO console choose	ToolIcon { iconId: "toolbar-back"; onClicked:  }
		ButtonRow {
			platformStyle: TabButtonStyle { }
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
			onClicked: {
				if (!romGallery.launch(romListModel.get(currentRomIndex))) {
					errorDialog.message = "Could not load machine plugin!"
					errorDialog.open()
				}
			}
		}
	}

	TabGroup {
		id: tabGroup
		currentTab: galleryTab

		GalleryPage { id: galleryTab }
		ListPage { id: listTab }

		onCurrentTabChanged: coverFlowEnabled = (currentTab === galleryTab)
	}

	Sheet {
		id: saveIconSheet
		property alias imgSource: img.source
		property alias imgScale: imageScaler.value

		property int iconX: 0
		property int iconY: 0

		// TODO waiting for new version of qt-components acceptButton.enabled when
		// icon selected -> rect.visible
		acceptButtonText: "Save"
		rejectButtonText: "Cancel"

		title: Label {
			anchors.centerIn: parent
			text: "Select Icon"
		}

		content: Item {
			anchors.fill: parent
			Label {
				width: parent.width
				text: "info: dialog creates icon in the home screen."
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Image {
				id: img
				anchors.centerIn: parent
				scale: imageScaler.value
			}
			Rectangle {
				id: rect
				x: img.x - (img.scale-1)*img.width/2 + saveIconSheet.iconX
				y: img.y - (img.scale-1)*img.height/2 + saveIconSheet.iconY
				width: 80
				height: 80
				color: Qt.rgba(1, 1, 1, 0.3)
				border.color: "white"
				border.width: 2
				visible: false
			}

			MouseArea {
				x: img.x - (img.scale-1)*img.width/2 + 40
				y: img.y - (img.scale-1)*img.height/2 + 40
				width: img.width*img.scale - 80
				height: img.height*img.scale - 80
				onClicked: {
					saveIconSheet.iconX = mouse.x - 40
					saveIconSheet.iconY = mouse.y - 40
					rect.visible = true
				}
			}
			Slider {
				id: imageScaler;
				anchors.right: parent.right
				height: parent.height
				orientation: Qt.Vertical
				minimumValue: 0.5; maximumValue: 4.0
				valueIndicatorVisible: true
				onValueChanged: rect.visible = false
			}
		}

		onAccepted: {
			if (!romGallery.addIconToHomeScreen(romListModel.get(currentRomIndex), imageScaler.value, iconX, iconY)) {
				errorDialog.message = "Could not save icon!"
				errorDialog.open()
			}
		}
	}

	QueryDialog {
		id: errorDialog
		icon: "image://theme/icon-m-common-red"
		message: ""
		acceptButtonText: "Close"
		rejectButtonText: ""
	}

	QueryDialog {
		id: howToInstallRomDialog

		acceptButtonText: ""
		rejectButtonText: "Close"

		titleText: "Help"
		message: "ROMs not found. To install ROM you need to attach phone to the PC and copy your files to \"emumaster/" + romListModel.machineName + "\" directory. Remember to detach phone from the PC. Consider a small donation if you find this software useful"
	}
	Component.onCompleted: {
		romListModel.machineName = "nes"
		if (romListModel.count > 0)
			currentRomIndex = 0
		else
			howToInstallRomDialog.open()
	}
	AboutSheet { id: aboutSheet }
}
