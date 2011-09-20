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
	content: Item {
		anchors.fill: parent
		Column {
			width: parent.width
			spacing: 50

			Label {
				width: parent.width
				text: "Author: Elemental\nWant new features? Found bug? Go to homepage"
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Image {
				source: "image://theme/icon-l-browser"
				anchors.horizontalCenter: parent.horizontalCenter

				MouseArea {
					anchors.fill: parent
					onClicked: romGallery.homepage()
				}
			}
			Label {
				width: parent.width
				text: "If you find this software useful please donate"
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Image {
				source: "../img/btn_donateCC_LG.png"
				scale: 2
				anchors.horizontalCenter: parent.horizontalCenter

				MouseArea {
					anchors.fill: parent
					onClicked: romGallery.donate()
				}
			}
		}
	}
}
