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
				id: swipeEnableItem
				width: parent.width

				Label {
					id: swipeEnableLabel
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Swipe Enabled")
					font.bold: true
				}
				Switch {
					id: swipeEnableSwitch
					anchors.right: parent.right
					checked: input.swipeEnable
					onCheckedChanged: input.swipeEnable = checked
					platformStyle: SwitchStyle { inverted: true }
				}
				Component.onCompleted: swipeEnableItem.height = Math.max(swipeEnableLabel.height, swipeEnableSwitch.height)
			}
		}
	}
}
