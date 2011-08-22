import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	SelectionDialog {
		id: sampleRateDialog
		titleText: "Select Sample Rate"
		model: ListModel {
			ListElement { name: "22050" }
			ListElement { name: "44100" }
		}
		onAccepted: {
			if (selectedIndex >= 0)
				audio.sampleRate = model.get(selectedIndex).name
		}
	}

	Flickable {
		id: flickable
		anchors.fill: parent
		contentWidth: col.width
		contentHeight: col.height
		flickableDirection: Flickable.VerticalFlick

		Column {
			id: col
			width: flickable.width
			spacing: 10

			Component.onCompleted: {
				var count = children.length;
				for (var i = 0; i < count; i++) {
					var item = children[i];
					item.anchors.horizontalCenter = item.parent.horizontalCenter;
				}
			}

			Button {
				text: (audio.enable ? qsTr("Disable") : qsTr("Enable")) + qsTr(" Audio")
				onClicked: audio.enable = !audio.enable
			}
			Button {
				id: stereoEnableButton
				text: qsTr("Stereo")
				checkable: true
				checked: audio.stereoEnable
				onClicked: audio.stereoEnable = !audio.stereoEnable
				visible: audio.enable
			}
			Button {
				text: qsTr("SampleRate ") + audio.sampleRate
				visible: audio.enable
				onClicked: sampleRateDialog.open()
			}
			ButtonColumn {
				exclusive: false
				visible: audio.enable

				Button {
					text: "Rectangle1 Channel"
					checkable: true
					checked: machine.apu.rectangle1Enable
					onClicked: machine.apu.rectangle1Enable = !machine.apu.rectangle1Enable
				}
				Button {
					text: "Rectangle2 Channel"
					checkable: true
					checked: machine.apu.rectangle2Enable
					onClicked: machine.apu.rectangle2Enable = !machine.apu.rectangle2Enable
				}
				Button {
					text: "Triangle Channel"
					checkable: true
					checked: machine.apu.triangleEnable
					onClicked: machine.apu.triangleEnable = !machine.apu.triangleEnable
				}
				Button {
					text: "Noise Channel"
					checkable: true
					checked: machine.apu.noiseEnable
					onClicked: machine.apu.noiseEnable = !machine.apu.noiseEnable
				}
				Button {
					text: "DMC Channel"
					checkable: true
					checked: machine.apu.dmcEnable
					onClicked: machine.apu.dmcEnable = !machine.apu.dmcEnable
				}
			}
		}
	}
}
