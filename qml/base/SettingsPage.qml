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

// TODO input devices config

Page {
	orientationLock: PageOrientation.LockPortrait
	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-back"
			onClicked: machineView.resume()
		}
	}

	Menu {
		id: stateMenu
		property int stateSlot

		function prepareAndOpen(index) {
			stateMenu.stateSlot = stateListModel.indexToSlot(index)
			stateMenu.open()
		}

		MenuLayout {
			MenuItem {
				text: qsTr("Load")
				onClicked: stateListModel.loadState(stateMenu.slot)
			}
			MenuItem {
				text: qsTr("Overwrite")
				onClicked: overwriteStateDialog.open()
			}
			MenuItem {
				text: qsTr("Delete")
				onClicked: deleteStateDialog.open()
			}
		}
	}

	Flickable {
		anchors.fill: parent
		flickableDirection: Flickable.VerticalFlick
		contentHeight: column.height+20

	Column {
		// TODO really y == 20 ??
		id: column
		y: 20
		width: parent.width
		height: childrenRect.height
		spacing: 20

		SectionSeperator { text: qsTr("STATE"); rightPad: 150 }
		ListView {
			id: stateListView
			width: parent.width
			height: 280
			model: stateListModel
			spacing: 8
			orientation: ListView.Horizontal

			delegate: ImageListViewDelegate {
				imgSource: "image://state/" + title + "*" + screenShotUpdate
				text: Qt.formatDateTime(saveDateTime, "dd.MM.yyyy hh:mm:ss")
				onClicked: stateMenu.prepareAndOpen(index)

				BorderImage {
					source: "image://theme/meegotouch-video-duration-background"
					anchors {
						right: parent.right; rightMargin: 30
						top: parent.top; topMargin: 20
					}
					width: childrenRect.width+20
					height: childrenRect.height+10
					visible: title == -2
					border { left: 10; right: 10 }

					Label { x: 10; y: 5; text: "AUTOSAVE"; color: "blue" }
				}
			}
		}
		Column {
			spacing: 5
			anchors.horizontalCenter: parent.horizontalCenter
			Button {
				text: qsTr("Save in New Slot")
				onClicked: {
					if (!stateListModel.saveState(-1))
						errorDialog.open()
					else
						stateListView.currentIndex = 0
				}
			}
			Button {
				text: qsTr("Reset System")
				onClicked: machine.reset()
			}
			Button {
				text: qsTr("Delete All")
				onClicked: deleteAllStateDialog.open()
			}
		}

		SectionSeperator { text: qsTr("INPUT"); rightPad: 150 }
		Label {
			id: padOpacityLabel
			text: qsTr("Pad Opacity")
			font.bold: true
		}
		Slider {
			id: padOpacitySlider
			width: parent.width
			minimumValue: 0.0
			maximumValue: 1.0
			value: machineView.padOpacity
			onValueChanged: machineView.padOpacity = value
			stepSize: 0.05
			valueIndicatorVisible: true
		}

		SectionSeperator { text: qsTr("VIDEO"); rightPad: 150 }
		Label {
			id: frameSkipLabel
			text: qsTr("Frameskip")
			font.bold: true
		}
		Slider {
			id: frameSkipSlider
			width: parent.width
			minimumValue: 0
			maximumValue: 5
			value: machineView.frameSkip
			onValueChanged: machineView.frameSkip = value
			stepSize: 1
			valueIndicatorVisible: true
		}
		EMSwitchOption {
			text: qsTr("Show FPS")
			checked: machineView.fpsVisible
			onCheckedChanged: machineView.fpsVisible = checked
		}
		EMSwitchOption {
			text: qsTr("Force Fullscreen")
			checked: !machineView.keepAspectRatio
			onCheckedChanged: machineView.keepAspectRatio = !checked
		}
		EMButtonOption {
			id: nesRenderMethodButton
			labelText: qsTr("PPU Render Method")
			onClicked: nesRenderMethodDialog.open()

			Component.onCompleted: {
				if (machine.name == "nes")
					nesRenderMethodButton.buttonText = nesRenderMethodModel.get(machine.ppu.renderMethod)["name"]
				else
					nesRenderMethodButton.visible = false
			}
		}

		SectionSeperator { text: qsTr("MISC"); rightPad: 150 }
		EMSwitchOption {
			text: qsTr("Audio Enabled")
			checked: machineView.audioEnable
			onCheckedChanged: machineView.audioEnable = checked
		}
		EMButtonOption {
			labelText: qsTr("Overwirte Image in Gallery")
			buttonText: qsTr("Take Screenshot")
			onClicked: machineView.saveScreenShot()
		}

		SectionSeperator {
			text: qsTr("ChEaTs"); rightPad: 150
			visible: machine.name == "nes"
		}
		Component.onCompleted: {
			if (machine.name == "nes") {
				var component = Qt.createComponent("NesCheatPage.qml")
				component.createObject(column)
			}
		}
	}

	}

	Connections {
		target: machineView
		onFaultOccured: {
			machineFaultDialog.message = faultStr
			machineFaultDialog.open()
		}
	}

	QueryDialog {
		id: overwriteStateDialog
		titleText: qsTr("Really?")
		message: qsTr("Do you really want to overwrite the state with current one?")
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		onAccepted: stateListModel.saveState(stateMenu.slot)
	}

	QueryDialog {
		id: deleteStateDialog
		titleText: qsTr("Really?")
		message: qsTr("Do you really want to delete the state?")
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		onAccepted: stateListModel.removeState(stateMenu.slot)
	}

	QueryDialog {
		id: deleteAllStateDialog
		titleText: qsTr("Really?")
		message: qsTr("Do you really want to delete all states?")
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		onAccepted: stateListModel.removeAll()
	}

	QueryDialog {
		id: errorDialog
		titleText: qsTr("Oops")
		message: qsTr("Something went wrong!")
		rejectButtonText: qsTr("Close")
	}
	QueryDialog {
		id: machineFaultDialog
		titleText: qsTr("Oops")
		rejectButtonText: qsTr("Close")
	}

	ListModel {
		id: nesRenderMethodModel
		ListElement { name: "Post All Render" }
		ListElement { name: "Pre All Render" }
		ListElement { name: "Post Render" }
		ListElement { name: "Pre Render" }
		ListElement { name: "Tile Render" }
	}
	SelectionDialog {
		id: nesRenderMethodDialog
		titleText: qsTr("Select Render Method")
		model: nesRenderMethodModel
		onAccepted: machine.ppu.renderMethod = selectedIndex
	}
}