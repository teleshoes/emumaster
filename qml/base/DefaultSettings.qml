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

Column {
	id: col
	width: parent.width
	spacing: 15

	EMSwitchOption {
		text: qsTr("Swipe Enabled")
		checked: machineView.swipeEnable; onCheckedChanged: machineView.swipeEnable = checked
	}
	Item {
		id: frameSkipItem
		width: parent.width
		Label {
			id: frameSkipLabel
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Frameskip ")
			font.bold: true
		}
		Slider {
			id: frameSkipSlider
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			minimumValue: 0
			maximumValue: 5
			value: machineView.frameSkip
			onValueChanged: machineView.frameSkip = value
			stepSize: 1
			valueIndicatorVisible: true
		}
		Label {
			anchors.right: frameSkipSlider.left
			anchors.rightMargin: 10
			anchors.verticalCenter: parent.verticalCenter
			text: frameSkipSlider.value
			font.bold: true
		}
		Component.onCompleted: frameSkipItem.height = Math.max(frameSkipLabel.height, frameSkipSlider.height)
	}
	EMSwitchOption {
		text: qsTr("Show FPS")
		checked: machineView.fpsVisible; onCheckedChanged: machineView.fpsVisible = checked
	}
	EMSwitchOption {
		text: qsTr("Keep Aspect Ratio")
		checked: machineView.keepAspectRatio; onCheckedChanged: machineView.keepAspectRatio = checked
	}
	EMSwitchOption {
		text: qsTr("Audio Enabled")
		checked: machineView.audioEnable; onCheckedChanged: machineView.audioEnable = checked
	}
	EMSwitchOption {
		text: qsTr("Use Accelerometer")
		checked: machineView.accelerometerEnable; onCheckedChanged: machineView.accelerometerEnable = checked
	}
	Item {
		id: padOpacityItem
		width: parent.width
		Label {
			id: padOpacityLabel
			anchors.verticalCenter: parent.verticalCenter
			text: qsTr("Pad Opacity")
			font.bold: true
		}
		Slider {
			id: padOpacitySlider
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			minimumValue: 0.0
			maximumValue: 1.0
			onValueChanged: machineView.padOpacity = value
			stepSize: 0.05
			valueIndicatorVisible: true
		}
		Component.onCompleted: {
			padOpacitySlider.value = machineView.padOpacity
			padOpacityItem.height = Math.max(padOpacityLabel.height, padOpacitySlider.height)
		}
	}
	EMButtonOption {
		labelText: qsTr("Overwirte Image in Gallery")
		buttonText: qsTr("Take Screenshot")
		onClicked: machineView.saveScreenShot()
	}
}
