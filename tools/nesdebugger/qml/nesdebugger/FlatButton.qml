import QtQuick 1.0

Rectangle {
	id: button
	width: label.width + 30; height: 20
	color: "transparent"
	border.color: "#c8c8c8"; border.width: 1

	property alias text: label.text
	signal clicked
	property alias iconSource: icon.source

	Item {
		anchors.centerIn: parent
		width: childrenRect.width; height: parent.height
		Image {
			id: icon
			anchors.verticalCenter: parent.verticalCenter
		}
		Text {
			id: label
			anchors.left: icon.right; anchors.leftMargin: 4
			height: button.height
			verticalAlignment: Text.AlignVCenter
		}
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
		onClicked: button.clicked()
	}
	states: [
		State {
			name: "hover"; when: mouseArea.containsMouse
			PropertyChanges { target: button; color: "#a4dbfb" }
		}
	]
}
