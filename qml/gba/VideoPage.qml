import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	Flickable {
		id: flickable
		x: 15
		width: parent.width - 30
		height: parent.height
		contentWidth: width
		contentHeight: col.height
		flickableDirection: Flickable.VerticalFlick

		Column {
			id: col
			width: parent.width
			spacing: 15

			Item {
				id: screenShotItem
				width: parent.width

				Label {
					id: screenShotLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Overwirte Image in ROM Gallery")
					font.bold: true
				}
				Button {
					id: screenShotButton
					anchors.right: parent.right
					text: qsTr("Take Screenshot")
					onClicked: machineView.saveScreenShot()
				}
				Component.onCompleted: screenShotItem.height = Math.max(screenShotLabel.height, screenShotButton.height)
			}
			Item {
				id: showFpsItem
				width: parent.width
				Label {
					id: showFpsLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Show FPS")
					font.bold: true
				}
				Switch {
					id: showFpsSwitch
					anchors.verticalCenter: parent.verticalCenter
					anchors.right: parent.right
					checked: video.fpsVisible
					onCheckedChanged: video.fpsVisible = checked
					platformStyle: SwitchStyle { inverted: true }
				}
				Component.onCompleted: showFpsItem.height = Math.max(showFpsLabel.height, showFpsSwitch.height)
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
					value: video.frameSkip
					onValueChanged: video.frameSkip = value
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
		}
	}
}
