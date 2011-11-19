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

Page {
	Flickable {
		id: flickable
		x: 15
		width: parent.width - 30
		height: parent.height
		contentWidth: width
		contentHeight: col.height
		flickableDirection: Flickable.VerticalFlick

		Column {
			id: col
			width: parent.width
			spacing: 15

			DefaultSettings {
				width: parent.width
			}
			EMButtonOption {
				id: nesRenderMethodButton
				labelText: qsTr("PPU Render Method")
				onClicked: nesRenderMethodDialog.open()

				Component.onCompleted: {
					if (machine.name == "nes")
						nesRenderMethodButton.buttonText = nesRenderMethodModel.get(machine.ppu.renderMethod)["name"]
					else
						nesRenderMethodButton.visible = false
				}
			}
			Button {
				id: cheatButton
				width: parent.width
				text: qsTr("ChEaTs")
				visible: machine.name == "nes"
				onClicked: appWindow.pageStack.push(Qt.resolvedUrl("NesCheatPage.qml"))
			}
		}
	}
	ListModel {
		id: nesRenderMethodModel
		ListElement { name: "Post All Render" }
		ListElement { name: "Pre All Render" }
		ListElement { name: "Post Render" }
		ListElement { name: "Pre Render" }
		ListElement { name: "Tile Render" }
	}
	SelectionDialog {
		id: nesRenderMethodDialog
		titleText: "Select Render Method"
		model: nesRenderMethodModel
		onAccepted: machine.ppu.renderMethod = selectedIndex
	}
}
