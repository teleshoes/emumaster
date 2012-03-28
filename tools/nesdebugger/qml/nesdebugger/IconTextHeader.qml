import QtQuick 1.1

Rectangle {
	property alias iconSource: img.source
	property alias text: label.text

	width: parent.width; height: 20
	border.width: 1; border.color: "#606060"
	color: "white"

	Item {
		width: childrenRect.width
		height: parent.height
		anchors.horizontalCenter: parent.horizontalCenter

		Image {
			id: img
			x: 4
			anchors.verticalCenter: parent.verticalCenter
		}
		Text {
			id: label
			anchors.left: img.right; anchors.leftMargin: 4
			anchors.verticalCenter: parent.verticalCenter
			font.pointSize: 8
			font.bold: true
		}
	}
}
