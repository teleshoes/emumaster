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

Page {
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
				onClicked: stateListModel.loadState(stateMenu.stateSlot)
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
		id: flickable
		anchors.fill: parent
		flickableDirection: Flickable.VerticalFlick
		contentHeight: column.height

	Column {
		id: column
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
				width: 380
				height: 280
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
		ButtonColumn {
			exclusive: false
			spacing: 5
			anchors.horizontalCenter: parent.horizontalCenter
			visible: appWindow.inPortrait

			Button {
				text: qsTr("Save in New Slot")
				onClicked: stateListModel.saveState(-1)
			}
			Button {
				text: qsTr("Restart")
				onClicked: restartDialog.open()
			}
			Button {
				text: qsTr("Delete All")
				onClicked: deleteAllStateDialog.open()
			}
		}
		ButtonRow {
			exclusive: false
			spacing: 5
			anchors.horizontalCenter: parent.horizontalCenter
			visible: !appWindow.inPortrait

			Button {
				text: qsTr("Save in New Slot")
				onClicked: stateListModel.saveState(-1)
			}
			Button {
				text: qsTr("Restart")
				onClicked: restartDialog.open()
			}
			Button {
				text: qsTr("Delete All")
				onClicked: deleteAllStateDialog.open()
			}
		}

		SectionSeperator { text: qsTr("INPUT"); rightPad: 150 }
		Label {
			text: qsTr("Pad Opacity")
			font.bold: true
		}
		Slider {
			width: parent.width
			minimumValue: 0.0
			maximumValue: 1.0
			value: machineView.padOpacity
			onValueChanged: machineView.padOpacity = value
			stepSize: 0.05
			valueIndicatorVisible: true
		}
		Repeater {
			id: inputDevicesView
			model: machineView.inputDevices

			Column {
				id: inputDeviceConfigurator
				width: parent.width
				spacing: 8

				property ListModel inputDeviceConfList: prepareModel()

				function prepareModel() {
					if (modelData.name == "touch")
						return inputTouchConfList
					else if (modelData.name == "accel")
						return inputAccelConfList
					else if (modelData.name == "keyb")
						return inputKeybConfList
					else if (modelData.name == "sixaxis")
						return inputSixAxisConfList
				}

				Item {
					width: parent.width
					height: childrenRect.height

					Image {
						id: inputDeviceIcon
						source: qsTr("../img/input-%1.png").arg(modelData.name)
					}

					Label {
						id: inputDeviceNameLabel
						anchors {
							left: inputDeviceIcon.right; leftMargin: 8
							verticalCenter: inputDeviceIcon.verticalCenter
						}
						text: {
							if (modelData.name == "touch")
								return qsTr("Touch Screen")
							else if (modelData.name == "accel")
								return qsTr("Accelerometer")
							else if (modelData.name == "keyb")
								return qsTr("Keyboard")
							else if (modelData.name == "sixaxis")
								return qsTr("SixAxis")
						}
						font.bold: true
					}
					Button {
						id: inputDeviceConfButton
						anchors.right: parent.right
						width: 250
						text: getConf()
						onClicked: {
							inputDeviceConfSelector.inputDevice = modelData
							inputDeviceConfSelector.model = inputDeviceConfigurator.inputDeviceConfList
							inputDeviceConfSelector.selectedIndex = modelData.confIndex
							inputDeviceConfSelector.open()
						}
						function getConf() {
							return inputDeviceConfigurator.inputDeviceConfList.get(modelData.confIndex)["name"]
						}
						Connections {
							target: modelData
							onConfIndexChanged: inputDeviceConfButton.text = inputDeviceConfButton.getConf()
						}
					}
				}
			}
		}

		SectionSeperator { text: qsTr("VIDEO"); rightPad: 150 }
		Label {
			text: qsTr("Frameskip")
			font.bold: true
		}
		Slider {
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
		EMSwitchOption {
			text: qsTr("Bilinear Filtering")
			checked: machineView.bilinearFiltering
			onCheckedChanged: machineView.bilinearFiltering = checked
		}
		EMButtonOption {
			id: nesRenderMethodButton
			labelText: qsTr("PPU Render Method")
			onClicked: nesRenderMethodDialog.open()

			Component.onCompleted: {
				if (machine.name === "nes")
					nesRenderMethodButton.refreshText()
				else
					nesRenderMethodButton.visible = false
			}
			function refreshText() {
				var text = nesRenderMethodModel.get(machine.ppu.renderMethod)["name"]
				nesRenderMethodButton.buttonText = text
			}
		}

		SectionSeperator { text: qsTr("MISC"); rightPad: 150 }
		EMSwitchOption {
			text: qsTr("Audio Enabled")
			checked: machineView.audioEnable
			onCheckedChanged: machineView.audioEnable = checked
		}
		EMButtonOption {
			labelText: qsTr("Overwrite Image in Gallery")
			buttonText: qsTr("Take Screenshot")
			onClicked: machineView.saveScreenShot()
		}

		SectionSeperator {
			text: qsTr("ChEaTs"); rightPad: 150
			visible: machine.name === "nes"
		}
		Component.onCompleted: {
			if (machine.name === "nes") {
				var component = Qt.createComponent("NesCheatPage.qml")
				component.createObject(column)
			}
		}
	}

	}
	ScrollDecorator { flickableItem: flickable }

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
		message: qsTr("Do you really want to overwrite the saved state with the current one?")
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		onAccepted: stateListModel.saveState(stateMenu.stateSlot)
	}

	QueryDialog {
		id: deleteStateDialog
		titleText: qsTr("Really?")
		message: qsTr("Do you really want to delete the saved state?")
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		onAccepted: stateListModel.removeState(stateMenu.stateSlot)
	}

	QueryDialog {
		id: deleteAllStateDialog
		titleText: qsTr("Really?")
		message: qsTr("Do you really want to delete all saved states?")
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		onAccepted: stateListModel.removeAll()
	}

	QueryDialog {
		id: restartDialog
		titleText: qsTr("Really?")
		message: qsTr("Do you really want to restart the emulated system?")
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		onAccepted: machine.reset()
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
		ListElement { name: QT_TR_NOOP("Post All Render") }
		ListElement { name: QT_TR_NOOP("Pre All Render") }
		ListElement { name: QT_TR_NOOP("Post Render") }
		ListElement { name: QT_TR_NOOP("Pre Render") }
		ListElement { name: QT_TR_NOOP("Tile Render") }
	}
	SelectionDialog {
		id: nesRenderMethodDialog
		titleText: qsTr("Select Render Method")
		model: nesRenderMethodModel
		onAccepted: {
			machine.ppu.renderMethod = selectedIndex
			nesRenderMethodButton.refreshText()
		}
	}
	ListModel {
		id: inputTouchConfList
		ListElement { name: QT_TR_NOOP("None") }
		ListElement { name: QT_TR_NOOP("Pad A") }
		ListElement { name: QT_TR_NOOP("Pad B") }
		ListElement { name: QT_TR_NOOP("Mouse A") }
		ListElement { name: QT_TR_NOOP("Mouse B") }
	}
	ListModel {
		id: inputAccelConfList
		ListElement { name: QT_TR_NOOP("None") }
		ListElement { name: QT_TR_NOOP("Pad A") }
		ListElement { name: QT_TR_NOOP("Pad B") }
	}
	ListModel {
		id: inputSixAxisConfList
		ListElement { name: QT_TR_NOOP("None") }
		ListElement { name: QT_TR_NOOP("Pad A") }
		ListElement { name: QT_TR_NOOP("Pad B") }
		ListElement { name: QT_TR_NOOP("Mouse A") }
		ListElement { name: QT_TR_NOOP("Mouse B") }
		ListElement { name: QT_TR_NOOP("Pad B + Mouse A") }
		ListElement { name: QT_TR_NOOP("Pad A + Mouse B") }
	}
	ListModel {
		id: inputKeybConfList
		ListElement { name: QT_TR_NOOP("None") }
		ListElement { name: QT_TR_NOOP("Pad A") }
		ListElement { name: QT_TR_NOOP("Pad B") }
		ListElement { name: QT_TR_NOOP("Keyboard") }
	}

	SelectionDialog {
		property variant inputDevice

		id: inputDeviceConfSelector
		model: []
		titleText: qsTr("Select Configuration")
		onAccepted: inputDevice.confIndex = selectedIndex
	}
}
