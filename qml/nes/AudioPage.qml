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
