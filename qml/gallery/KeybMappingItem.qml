import QtQuick 1.1
import com.nokia.meego 1.0

Item {
	id: keybMappingItem

    property alias buttonName: itemLabel.text
    property alias hostKeyName: itemButton.text

	signal clicked

	width: parent.width
	height: childrenRect.height
	Label {
		id: itemLabel
	}
	Button {
		id: itemButton
		anchors.right: parent.right
		onClicked: keybMappingItem.clicked()
	}
}
