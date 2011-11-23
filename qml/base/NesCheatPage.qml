/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import "../base"
import "constants.js" as UI

Item {
	width: parent.width
	height: childrenRect.height

	Column {
		width: parent.width

	ButtonRow {
		exclusive: false

		Button {
			text: qsTr("Add")
			onClicked: addCheatSheet.open()
		}
		Button {
			text: qsTr("Remove")
			onClicked: gameGenie.removeAt(listView.currentIndex)
		}
	}
	ListView {
		id: listView
		width: parent.width
		height: gameGenie.count * UI.LIST_ITEM_HEIGHT
		model: gameGenie
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
				onCheckedChanged: gameGenie.setEnabled(index, checked)
			}
		}
	}

	}
	Sheet {
		id: addCheatSheet

		// TODO meego version acceptButton.enabled: false
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
					// TODO error highlight not updated
					id: codeEdit
					anchors { left: parent.left; right: parent.right }
					errorHighlight: true
					inputMethodHints: Qt.ImhUppercaseOnly | Qt.ImhNoPredictiveText
					// TODO new components version onAccepted: descriptionEdit.focus = true
					onTextChanged: {
						var codeOk = gameGenie.isCodeValid(text)
						// TODO new components version addCheatSheet.acceptButton.enabled = codeOk
						errorHighlight = !codeOk
					}
				}

				Label { text: qsTr("Description:") }
				TextField {
					id: descriptionEdit
					anchors { left: parent.left; right: parent.right }
					// TODO meego version onAccepted: addCheatSheet.accept()
				}
			}
		}

		onAccepted: {
			if (gameGenie.isCodeValid(codeEdit.text))
				gameGenie.addNew(codeEdit.text, descriptionEdit.text)
		}
	}
}
