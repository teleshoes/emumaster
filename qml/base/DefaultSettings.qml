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
		text: qsTr("Exit Button")
		checked: machineView.quickQuitEnable; onCheckedChanged: machineView.quickQuitEnable = checked
	}
	EMSwitchOption {
		text: qsTr("Audio Enabled")
		checked: machineView.audioEnable; onCheckedChanged: machineView.audioEnable = checked
	}
	EMSwitchOption {
		text: qsTr("Keep Aspect Ratio")
		checked: machineView.keepAspectRatio; onCheckedChanged: machineView.keepAspectRatio = checked
	}
	EMButtonOption {
		labelText: qsTr("Overwirte Image in Gallery")
		buttonText: qsTr("Take Screenshot")
		onClicked: machineView.saveScreenShot()
	}
}
