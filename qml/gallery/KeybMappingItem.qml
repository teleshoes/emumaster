import QtQuick 1.1
import com.nokia.meego 1.0

Item {
	property alias keyText: itemLabel.text
	property alias hostKeyText: itemButton.text

	width: parent.width
	height: childrenRect.height
	Label {
		id: itemLabel
		text:
	}
	Button {
		id: itemButton
		anchors.right: parent.right
	}
}
