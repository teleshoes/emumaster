/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import "constants.js" as UI

Item {
	width: parent.width
	height: childrenRect.height

	Column {
		width: parent.width

	ButtonRow {
		anchors.horizontalCenter: parent.horizontalCenter
		exclusive: false

		Button {
			text: qsTr("Add")
			onClicked: addCheatSheet.open()
		}
		Button {
			text: qsTr("Remove")
			onClicked: emu.gameGenie.removeAt(listView.currentIndex)
		}
	}
	ListView {
		id: listView
		width: parent.width
		height: emu.gameGenie.count * UI.LIST_ITEM_HEIGHT
		model: emu.gameGenie
		delegate: MyListDelegate {
			id: listViewDelegate
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
				onCheckedChanged: emu.gameGenie.setEnabled(index, checked)
			}
		}
	}

	}
	Sheet {
		id: addCheatSheet

		acceptButton.enabled: false
		acceptButtonText: qsTr("Add")
		rejectButtonText: qsTr("Cancel")

		content: Item {
			anchors.fill: parent
			Column {
				y:10
				spacing: 10
				width: parent.width

				Label { text: qsTr("Game Genie Code:") }
				TextField {
					id: codeEdit
					anchors { left: parent.left; right: parent.right }
					errorHighlight: true
					inputMethodHints: Qt.ImhUppercaseOnly | Qt.ImhNoPredictiveText
					onAccepted: descriptionEdit.focus = true
					onTextChanged: {
						var codeOk = emu.gameGenie.isCodeValid(text)
						addCheatSheet.acceptButton.enabled = codeOk
						errorHighlight = !codeOk
					}
				}

				Label { text: qsTr("Description:") }
				TextField {
					id: descriptionEdit
					anchors { left: parent.left; right: parent.right }
					onAccepted: addCheatSheet.accept()
				}
			}
		}

		onAccepted: emu.gameGenie.addNew(codeEdit.text, descriptionEdit.text)
	}
}
