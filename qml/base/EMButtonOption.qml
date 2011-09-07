import QtQuick 1.1
import com.nokia.meego 1.0

Item {
	property alias labelText: label.text
	property alias buttonText: button.text
	signal clicked

	id: item
	width: parent.width

	Label {
		id: label
		anchors.verticalCenter: parent.verticalCenter
		font.bold: true
	}
	Button {
		id: button
		anchors.right: parent.right
		onClicked: item.clicked()
	}
	Component.onCompleted: item.height = Math.max(label.height, button.height)
}
