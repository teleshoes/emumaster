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
				text: qsTr("Audio Enabled")
				checked: audio.enable; onCheckedChanged: machineView.enable = checked
			}
			EMButtonOption {
				labelText: qsTr("Audio Sample Rate")
				buttonText: machineView.sampleRate
				onClicked: sampleRateDialog.open()
			}
			EMButtonOption {
				labelText: qsTr("Overwirte Image in Gallery")
				buttonText: qsTr("Take Screenshot")
				onClicked: machineView.saveScreenShot()
			}
			EMButtonOption {
				labelText: qsTr("PPU Render Method")
				buttonText: renderMethodModel.get(machine.ppu.renderMethod)["name"]
				onClicked: renderMethodDialog.open()
			}
			EMSwitchOption {
				text: qsTr("PPU Sprite Clipping")
				checked: machine.ppu.spriteClippingEnable; onCheckedChanged: machine.ppu.spriteClippingEnable = checked
			}
		}
	}
	SelectionDialog {
		id: sampleRateDialog
		titleText: "Select Sample Rate"
		model: ListModel {
			ListElement { name: "22050" }
			ListElement { name: "44100" }
		}
		onAccepted: machineView.sampleRate = model.get(selectedIndex).name
	}
	ListModel {
		id: renderMethodModel
		ListElement { name: "Post All Render" }
		ListElement { name: "Pre All Render" }
		ListElement { name: "Post Render" }
		ListElement { name: "Pre Render" }
		ListElement { name: "Tile Render" }
	}
	SelectionDialog {
		id: renderMethodDialog
		titleText: "Select Render Method"
		model: renderMethodModel
		onAccepted: machine.ppu.renderMethod = selectedIndex
	}
}
