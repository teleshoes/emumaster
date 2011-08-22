import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

Page {
	ListView {
		id: listView
		anchors.fill: parent
		currentIndex: romChooserPage.currentRomIndex

		delegate: MyListDelegate {
			id: delegate

			BorderImage {
				id: overlay
				anchors.fill: parent
				source: "image://theme/meegotouch-panel-background-selected"
				opacity: 0.5
				visible: false
			}

			onClicked: {
				romChooserPage.currentRomIndex = index
			}

			states: [
				State {
					name: "selected"; when: listView.currentIndex === index
					PropertyChanges { target: overlay; visible: true }
				}
			]
		}

		section.property: "alphabet"
		section.criteria: ViewSection.FullString
		section.delegate: Item {
			width: parent.width
			height: 40
			Text {
				id: headerLabel
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				anchors.rightMargin: 8
				anchors.bottomMargin: 2
				text: section
				font.bold: true
				font.pointSize: 18
				color: theme.inverted ? "#4D4D4D" : "#3C3C3C";
			}
			Image {
				anchors.right: headerLabel.left
				anchors.left: parent.left
				anchors.verticalCenter: headerLabel.verticalCenter
				anchors.rightMargin: 24
				source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
			}
		}
	}
	MySectionScroller {
		listView: listView
	}
	ScrollDecorator {
		flickableItem: listView
	}
	Component.onCompleted: {
		listView.model = romListModel
	}
}
