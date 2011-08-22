import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	Row {
		anchors.centerIn: parent

		Button {
			width: 400
			text: (input.swipeEnable ? "Disable" : "Enable") + " Swipe in Game"
			onClicked: input.swipeEnable = !input.swipeEnable
		}
	}
}
