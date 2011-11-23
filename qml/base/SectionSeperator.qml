import QtQuick 1.1
import com.nokia.meego 1.0

Item {
	id: sectionDelegate
	property int rightPad: 80
	property alias text: headerLabel.text

	width: parent.width
	height: 40
	Text {
		id: headerLabel
		width: sectionDelegate.rightPad
		height: parent.height
		anchors.right: parent.right
		font.pointSize: 18
		color: theme.inverted ? "#4D4D4D" : "#3C3C3C";
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
	}
	Image {
		anchors.left: parent.left
		anchors.leftMargin: 10
		anchors.right: parent.right
		anchors.rightMargin: sectionDelegate.rightPad
		anchors.verticalCenter: headerLabel.verticalCenter
		source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
	}
}
