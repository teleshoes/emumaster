import QtQuick 1.1
import com.nokia.meego 1.0

Sheet {
	id: aboutSheet

	acceptButtonText: ""
	rejectButtonText: "Close"

	title: Label {
		anchors.centerIn: parent
		text: "About"
	}
// TODO homepage
// TODO bug report
	content: Item {
		anchors.fill: parent
		Label {
			width: parent.width
			text: "Author: Marcin Kaźmierczak\nIf you find this software useful please donate\n"
			wrapMode: Text.WordWrap
			horizontalAlignment: Text.AlignHCenter
		}
		Image {
			id: img
			anchors.centerIn: parent
			source: "../img/btn_donateCC_LG.png"
			scale: 2

			MouseArea {
				anchors.fill: parent
				onClicked: romGallery.donate()
			}
		}
	}
}
