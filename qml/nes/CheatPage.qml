import QtQuick 1.1
import com.nokia.meego 1.0
import "../common"

Page {
	ListView {
		id: listView
		anchors.fill: parent
		model: gameGenieCodeListModel
		delegate: MyListDelegate {
			title: code
			subtitle: description
			subtitleSize: 16

			BorderImage {
				id: overlay
				anchors.fill: parent
				source: "image://theme/meegotouch-panel-background-selected"
				opacity: 0.5
				visible: false
			}
			onClicked: listView.currentIndex = index

			states: [
				State {
					name: "selected"; when: listView.currentIndex === index
					PropertyChanges { target: overlay; visible: true }
				}
			]
			Switch {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: 20
				checked: isEnabled
				platformStyle: SwitchStyle { inverted: true }
				onCheckedChanged: gameGenieCodeListModel.setEnabled(index, checked)
			}
		}
		ButtonRow {
			anchors.bottom: parent.bottom
			anchors.horizontalCenter: parent.horizontalCenter
			exclusive: false

			Button {
				text: "Add New"
				onClicked: addCheatSheet.open()
			}
			Button {
				text: "Remove"
				onClicked: gameGenieCodeListModel.removeAt(listView.currentIndex)
			}
		}
	}
	Sheet {
		id: addCheatSheet

		// TODO meego version acceptButton.enabled: false
		acceptButtonText: "Add"
		rejectButtonText: "Cancel"

		title: Label {
			anchors.centerIn: parent
			text: "Add Code"
		}

		content: Item {
			anchors.fill: parent
			Column {
				width: parent.width

				Label {
					text: "Game Genie Code:"
				}
				TextField {
					// TODO error highlight not updated
					id: codeEdit
					anchors {left: parent.left; right: parent.right;}
					errorHighlight: true
					inputMethodHints: Qt.ImhPreferUppercase | Qt.ImhNoPredictiveText
					// TODO meego version onAccepted: descriptionEdit.focus = true
					onTextChanged: {
						var codeOk = machineView.isGameGenieCodeValid(text)
						addCheatSheet.acceptButton.enabled = codeOk
						errorHighlight = !codeOk
					}
				}

				Label {
					text: "Description:"
				}
				TextField {
					id: descriptionEdit
					anchors {left: parent.left; right: parent.right;}
					// TODO meego version onAccepted: addCheatSheet.accept()
				}
			}
		}

		onAccepted: gameGenieCodeListModel.addNew(codeEdit.text, descriptionEdit.text)
	}
}
