import QtQuick 1.1
import com.nokia.meego 1.0

Item {
	id: switchItem
	width: parent.width

	property alias text: switchLabel.text
	property alias checked: enableSwitch.checked

	Label {
		id: switchLabel
		anchors.verticalCenter: parent.verticalCenter
		font.bold: true
	}
	Switch {
		id: enableSwitch
		anchors.right: parent.right
		platformStyle: SwitchStyle { inverted: true }
	}
	Component.onCompleted: switchItem.height = Math.max(switchLabel.height, enableSwitch.height)
}
