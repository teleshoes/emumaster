/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	id: settingsPage
	property alias columnContent: column.children
	property alias inputContent: inputColumn.children
	property alias videoContent: videoColumn.children
	property alias miscContent: miscColumn.children
	property string initiatedAction

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-back"
			onClicked: emuView.showEmulationView()
		}
	}

	function yesNoDialogMessage(action) {
		switch (action) {
		case "overwriteState":	return qsTr("Do you really want to overwrite the saved state with the current one?")
		case "deleteState":		return qsTr("Do you really want to delete the saved state?")
		case "deleteAllStates":	return qsTr("Do you really want to delete all saved states?")
		case "emuReset":		return qsTr("Do you really want to restart the emulated system?")
		default:				return qsTr("Unknown action: "+action+", please send info to the developer!")
		}
	}

	function executeAction(action) {
		switch (action) {
		case "loadState":
			stateListModel.loadState(stateListView.selectedSlot);
			emuView.showEmulationView()
			break
		case "saveState":		stateListModel.saveState(-1); break
		case "overwriteState":	stateListModel.saveState(stateListView.selectedSlot); break
		case "deleteState":		stateListModel.removeState(stateListView.selectedSlot); break
		case "deleteAllStates":	stateListModel.removeAll(); break
		case "emuReset":		emu.reset(); break
		default:				console.log("unknown action: " + action); break
		}
	}

	function initAction(action) {
		switch (action) {
		case "loadState":
		case "saveState":
			settingsPage.executeAction(action); break
		default:
			settingsPage.initiatedAction = action
			yesNoDialog.message = yesNoDialogMessage(action)
			yesNoDialog.open()
			break
		}
	}

	Menu {
		id: stateMenu

		MenuLayout {
			MenuItem { text: qsTr("Load");		onClicked: settingsPage.initAction("loadState") }
			MenuItem { text: qsTr("Overwrite");	onClicked: settingsPage.initAction("overwriteState") }
			MenuItem { text: qsTr("Delete");	onClicked: settingsPage.initAction("deleteState") }
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

			SectionSeperator { text: qsTr("STATE") }
			ListView {
				property int selectedSlot: -3

				id: stateListView
				width: parent.width
				height: 280
				model: stateListModel
				spacing: 8
				orientation: ListView.Horizontal

				delegate: ImageListViewDelegate {
					width: 380
					height: 280
					imgSource: "image://state/" + slot + "*" + screenShotUpdate
					text: Qt.formatDateTime(saveDateTime, "dd.MM.yyyy hh:mm:ss")
					onPressed: stateListView.selectedSlot = slot
					onClicked: stateMenu.open()

					BorderImage {
						source: "image://theme/meegotouch-video-duration-background"
						anchors {
							right: parent.right; rightMargin: 30
							top: parent.top; topMargin: 20
						}
						width: childrenRect.width+20
						height: childrenRect.height+10
						visible: slot == -2
						border { left: 10; right: 10 }

						Label { x: 10; y: 5; text: qsTr("AUTOSAVE"); color: "blue" }
					}
				}
			}
			ButtonColumn {
				exclusive: false
				spacing: 5
				anchors.horizontalCenter: parent.horizontalCenter
				visible: appWindow.inPortrait

				Button { text: qsTr("Save in New Slot");	onClicked: settingsPage.initAction("saveState") }
				Button { text: qsTr("Restart");				onClicked: settingsPage.initAction("emuReset") }
				Button { text: qsTr("Delete All");			onClicked: settingsPage.initAction("deleteAllStates") }
			}
			ButtonRow {
				exclusive: false
				spacing: 5
				anchors.horizontalCenter: parent.horizontalCenter
				visible: !appWindow.inPortrait

				Button { text: qsTr("Save in New Slot");	onClicked: settingsPage.initAction("saveState") }
				Button { text: qsTr("Restart");				onClicked: settingsPage.initAction("emuReset") }
				Button { text: qsTr("Delete All");			onClicked: settingsPage.initAction("deleteAllStates") }
			}

			Column {
				id: inputColumn
				width: parent.width
				height: childrenRect.height
				spacing: 20

				SectionSeperator { text: qsTr("INPUT") }
				Label { text: qsTr("Pad Opacity") }
				Slider {
					width: parent.width
					minimumValue: 0.0
					maximumValue: 1.0
					value: emuView.padOpacity
					onValueChanged: emuView.padOpacity = value
					stepSize: 0.05
					valueIndicatorVisible: true
				}
				Repeater {
					id: inputDevicesView
					model: emuView.inputDevices

					Item {
						width: parent.width
						height: childrenRect.height

						Image {
							id: inputDeviceIcon
							source: qsTr("../img/input-%1.png").arg(modelData.shortName)
						}
						Label {
							id: inputDeviceNameLabel
							anchors {
								left: inputDeviceIcon.right; leftMargin: 8
								verticalCenter: inputDeviceIcon.verticalCenter
							}
							text: modelData.name
						}
						Button {
							id: inputDeviceFunctionButton
							anchors.right: parent.right
							width: 250
							text: modelData.emuFunctionName
							onClicked: {
								inputDeviceFunctionSelector.model = modelData.emuFunctionNameList
								inputDeviceFunctionSelector.selectedIndex = modelData.emuFunction
								inputDeviceFunctionSelector.open()
							}
						}
					}
				}
			}

			Column {
				id: videoColumn
				width: parent.width
				height: childrenRect.height
				spacing: 20

				SectionSeperator { text: qsTr("VIDEO") }
				Label { text: qsTr("Frameskip") }
				Slider {
					width: parent.width
					minimumValue: 0
					maximumValue: 5
					value: emuView.frameSkip
					onValueChanged: emuView.frameSkip = value
					stepSize: 1
					valueIndicatorVisible: true
				}
				SettingsSwitchItem { text: qsTr("Show FPS"); paramName: "fpsVisible" }
				SettingsSwitchItem { text: qsTr("Keep Aspect Ratio"); paramName: "keepAspectRatio" }
				SettingsSwitchItem { text: qsTr("Bilinear Filtering"); paramName: "bilinearFiltering" }
			}

			Column {
				id: miscColumn
				width: parent.width
				height: childrenRect.height
				spacing: 20

				SectionSeperator { text: qsTr("MISC") }
				SettingsSwitchItem { text: qsTr("Audio Enabled"); paramName: "audioEnable" }
				EMButtonOption {
					labelText: qsTr("Overwrite Image in Gallery")
					buttonText: qsTr("Take Screenshot")
					onClicked: emuView.saveScreenShot()
				}
			}
		}

	}
	ScrollDecorator { flickableItem: flickable }

	Connections {
		target: emuView
		onFaultOccured: {
			emuFaultDialog.message = faultMessage
			emuFaultDialog.open()
		}
	}

	QueryDialog {
		id: yesNoDialog
		titleText: qsTr("Really?")
		acceptButtonText: qsTr("Yes")
		rejectButtonText: qsTr("No")
		onAccepted: settingsPage.executeAction(settingsPage.initiatedAction)
	}

	QueryDialog {
		id: emuFaultDialog
		titleText: qsTr("Oops")
		rejectButtonText: qsTr("Close")
	}
	SelectionDialog {
		id: inputDeviceFunctionSelector
		titleText: qsTr("Select Configuration")
		onAccepted: inputDevice.emuFunction = selectedIndex
	}
}
