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
			width: flickable.width
			spacing: 15

			Item {
				id: audioEnableItem
				width: parent.width

				Label {
					id: audioEnableLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Audio Enabled")
					font.bold: true
				}
				Switch {
					id: audioEnableSwitch
					anchors.right: parent.right
					checked: audio.enable
					onCheckedChanged: audio.enable = checked
				}
				Component.onCompleted: audioEnableItem.height = Math.max(audioEnableLabel.height, audioEnableSwitch.height)
			}
			Item {
				id: stereoEnableItem
				width: parent.width
				visible: audio.enable

				Label {
					id: stereoEnableLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Stereo Enabled")
					font.bold: true
				}
				Switch {
					id: stereoEnableSwitch
					anchors.right: parent.right
					checked: audio.stereoEnable
					onCheckedChanged: audio.stereoEnable = checked
				}
				Component.onCompleted: stereoEnableItem.height = Math.max(stereoEnableLabel.height, stereoEnableSwitch.height)
			}
			Item {
				id: sampleRateItem
				width: parent.width
				visible: audio.enable

				Label {
					id: sampleRateLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Sample Rate")
					font.bold: true
				}
				Button {
					id: sampleRateButton
					anchors.right: parent.right
					text: audio.sampleRate
					onClicked: sampleRateDialog.open()
				}
				Component.onCompleted: sampleRateItem.height = Math.max(sampleRateLabel.height, sampleRateButton.height)
			}
			Item {
				id: rectangle1ChannelItem
				width: parent.width
				visible: audio.enable

				Label {
					id: rectangle1ChannelLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Rectangle 1 Channel")
					font.bold: true
				}
				Switch {
					id: rectangle1ChannelSwitch
					anchors.right: parent.right
					checked: machine.apu.rectangle1Enable
					onCheckedChanged: machine.apu.rectangle1Enable = checked
				}
				Component.onCompleted: rectangle1ChannelItem.height = Math.max(rectangle1ChannelLabel.height, rectangle1ChannelSwitch.height)
			}
			Item {
				id: rectangle2ChannelItem
				width: parent.width
				visible: audio.enable

				Label {
					id: rectangle2ChannelLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Rectangle 2 Channel")
					font.bold: true
				}
				Switch {
					id: rectangle2ChannelSwitch
					anchors.right: parent.right
					checked: machine.apu.rectangle2Enable
					onCheckedChanged: machine.apu.rectangle2Enable = checked
				}
				Component.onCompleted: rectangle2ChannelItem.height = Math.max(rectangle2ChannelLabel.height, rectangle2ChannelSwitch.height)
			}
			Item {
				id: triangleChannelItem
				width: parent.width
				visible: audio.enable

				Label {
					id: triangleChannelLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Triangle Channel")
					font.bold: true
				}
				Switch {
					id: triangleChannelSwitch
					anchors.right: parent.right
					checked: machine.apu.triangleEnable
					onCheckedChanged: machine.apu.triangleEnable = checked
				}
				Component.onCompleted: triangleChannelItem.height = Math.max(triangleChannelLabel.height, triangleChannelSwitch.height)
			}
			Item {
				id: noiseChannelItem
				width: parent.width
				visible: audio.enable

				Label {
					id: noiseChannelLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Noise Channel")
					font.bold: true
				}
				Switch {
					id: noiseChannelSwitch
					anchors.right: parent.right
					checked: machine.apu.noiseEnable
					onCheckedChanged: machine.apu.noiseEnable = checked
				}
				Component.onCompleted: noiseChannelItem.height = Math.max(noiseChannelLabel.height, noiseChannelSwitch.height)
			}
			Item {
				id: dmcChannelItem
				width: parent.width
				visible: audio.enable

				Label {
					id: dmcChannelLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("DMC Channel")
					font.bold: true
				}
				Switch {
					id: dmcChannelSwitch
					anchors.right: parent.right
					checked: machine.apu.dmcEnable
					onCheckedChanged: machine.apu.dmcEnable = checked
				}
				Component.onCompleted: dmcChannelItem.height = Math.max(dmcChannelLabel.height, dmcChannelSwitch.height)
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
		onAccepted: audio.sampleRate = model.get(selectedIndex).name
	}
}
