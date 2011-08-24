import QtQuick 1.1
import com.nokia.meego 1.0
// TODO reset button
Page {
	Row {
		anchors.centerIn: parent
		spacing: 100

		Column {
			spacing: 10

			Button {
				text: qsTr("Save Screenshot")
				onClicked: machineView.saveScreenShot()
			}
			Button {
				text: qsTr("Show FPS")
				checkable: true
				checked: video.fpsVisible
				onClicked: video.fpsVisible = !video.fpsVisible
			}
		}
		Column {
			spacing: 10

			Item {
				width: 300
				height: 20

				Rectangle {
					width: childrenRect.width
					height: childrenRect.height
					radius: 10
					color: "#e0e1e2"

					Row {
						x: 10
						spacing: 20

						Label {
							text: qsTr("Frameskip")
							font.pixelSize: 30
							font.bold: true
						}
						Label {
							text: fsSlider.value
							font.pixelSize: 30
							font.bold: true

						}
					}
				}
			}
			Slider {
				id: fsSlider
				width: 320
				minimumValue: 0
				maximumValue: 5
				value: video.frameSkip
				onValueChanged: video.frameSkip = value
				stepSize: 1
			}
			Button {
				text: qsTr("Sprite Clipping")
				checkable: true
				checked: machine.ppu.spriteClippingEnable
				onClicked: machine.ppu.spriteClippingEnable = !machine.ppu.spriteClippingEnable
			}
		}
	}
}
