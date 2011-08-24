import QtQuick 1.1
import com.nokia.meego 1.0
import "../common"

Page {
	ListView {
		id: listView
		anchors.fill: parent
		model: gameGenieCodeListModel
		delegate: MyListDelegate {
			title: code+"-"+description
			titleSize: 16

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
			Row {
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: 20

				Switch {
					checked: isEnabled
					platformStyle: SwitchStyle { inverted: true }
					onCheckedChanged: gameGenieCodeListModel.setEnabled(index, checked)
				}
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

		acceptButton.enabled: false
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
					font: UiConstants.FieldLabelFont
					color: UiConstants.FieldLabelColor
					text: "Game Genie Code:"
				}
				TextField {
					id: codeEdit
					anchors {left: parent.left; right: parent.right;}
					errorHighlight: true
					inputMethodHints: Qt.ImhPreferUppercase | Qt.ImhNoPredictiveText
					onAccepted: descriptionEdit.focus = true
					onTextChanged: {
						var codeOk = machineView.isGameGenieCodeValid(text)
						addCheatSheet.acceptButton.enabled = codeOk
						errorHighlight = !codeOk
					}
				}

				Label {
					font: UiConstants.FieldLabelFont
					color: UiConstants.FieldLabelColor
					text: "Description:"
				}
				TextField {
					id: descriptionEdit
					anchors {left: parent.left; right: parent.right;}
					onAccepted: addCheatSheet.accept()
				}
			}
		}

		onAccepted: gameGenieCodeListModel.addNew(codeEdit.text, descriptionEdit.text)
	}
}
