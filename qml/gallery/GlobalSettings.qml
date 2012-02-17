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
			onClicked: appWindow.pageStack.pop()
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

		GlobalSettingsSwitchItem { text: qsTr("Auto Load/Save on Start/Exit"); optionName: "autoSaveLoadEnable" }

		SectionSeperator { text: qsTr("INPUT") }

		GlobalSettingsSwitchItem { text: qsTr("Swipe Enabled"); optionName: "swipeEnable" }

		Label { text: qsTr("Pad Opacity") }
		Slider {
			width: parent.width
			minimumValue: 0.0
			maximumValue: 1.0
			value: diskGallery.globalOption("padOpacity")
			onValueChanged: diskGallery.setGlobalOption("padOpacity", value)
			stepSize: 0.05
			valueIndicatorVisible: true
		}
		Button {
			text: qsTr("Keyboard Configuration")
			anchors.horizontalCenter: parent.horizontalCenter
			onClicked: appWindow.pageStack.push(Qt.resolvedUrl("KeybMappingPage.qml"))
		}
		Button {
			text: qsTr("Calibrate Accelerometer")
			anchors.horizontalCenter: parent.horizontalCenter
			onClicked: appWindow.pageStack.push(Qt.resolvedUrl("AccelCalibrationPage.qml"))
		}

		SectionSeperator { text: qsTr("VIDEO") }

		Label { text: qsTr("Frameskip") }
		Slider {
			width: parent.width
			minimumValue: 0
			maximumValue: 5
			value: diskGallery.globalOption("frameSkip")
			onValueChanged: diskGallery.setGlobalOption("frameSkip", value)
			stepSize: 1
			valueIndicatorVisible: true
		}
		GlobalSettingsSwitchItem { text: qsTr("Show FPS"); optionName: "fpsVisible" }
		GlobalSettingsSwitchItem { text: qsTr("Keep Aspect Ratio"); optionName: "keepAspectRatio" }
		GlobalSettingsSwitchItem { text: qsTr("Bilinear Filtering"); optionName: "bilinearFiltering" }

		SectionSeperator { text: qsTr("MISC") }

		GlobalSettingsSwitchItem { text: qsTr("Audio Enabled"); optionName: "audioEnable" }

		EMSwitchOption {
			id: runInBackgroundSwitch
			text: qsTr("Run in Background")
			checked: diskGallery.globalOption("runInBackground")
			onCheckedChanged: {
				if (checked) {
					// special handling to prevent the dialog from showing at the start
					if (diskGallery.globalOption("runInBackground") !== "true")
						runInBackgroundDialog.open()
				} else {
					diskGallery.setGlobalOption("runInBackground", false)
				}
			}
		}
	}

	}
	ScrollDecorator { flickableItem: flickable }

	QueryDialog {
		id: runInBackgroundDialog
		titleText: qsTr("Enabling the option will run the application in the background. " +
						"This can consume a lot of battery!")
		acceptButtonText: qsTr("Enable")
		rejectButtonText: qsTr("Disable")
		onRejected: runInBackgroundSwitch.checked = false
		onAccepted: diskGallery.setGlobalOption("runInBackground", true)
	}
}
