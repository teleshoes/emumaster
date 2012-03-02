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
import com.nokia.meego 1.1
import "../base"

AdvancedLaunchPage {
	SelectionItem {
		titleText: qsTr("TV Encoding System")
		subtitleText: tvSystemModel.get(tvSystemDialog.selectedIndex).name
		onClicked: tvSystemDialog.open()
	}

	SelectionItem {
		titleText: qsTr("Render Method")
		subtitleText: renderMethodModel.get(renderMethodDialog.selectedIndex).name
		onClicked: renderMethodDialog.open()
	}

	children: [
		SelectionDialog {
			id: renderMethodDialog
			model: ListModel {
				id: renderMethodModel
				ListElement { name: QT_TR_NOOP("Auto") }
				ListElement { name: QT_TR_NOOP("Post All Render") }
				ListElement { name: QT_TR_NOOP("Pre All Render") }
				ListElement { name: QT_TR_NOOP("Post Render") }
				ListElement { name: QT_TR_NOOP("Pre Render") }
				ListElement { name: QT_TR_NOOP("Tile Render") }
			}
			titleText: qsTr("Select Render Method")
			selectedIndex: 0
		},
		SelectionDialog {
			id: tvSystemDialog
			model: ListModel {
				id: tvSystemModel
				ListElement { name: QT_TR_NOOP("Auto") }
				ListElement { name: QT_TR_NOOP("NTSC") }
				ListElement { name: QT_TR_NOOP("PAL") }
			}
			titleText: qsTr("Select TV Encoding System")
			selectedIndex: 0
		}
	]

	function confString() {
		var str = ""
		if (tvSystemDialog.selectedIndex > 0) {
			str += "nes.tvSystem="
			if (tvSystemDialog.selectedIndex == 2)
				str += "PAL"
			else
				str += "NTSC"
		}
		if (renderMethodDialog.selectedIndex > 0) {
			str += ","
			str += "nes.renderMethod="+(renderMethodDialog.selectedIndex-1)
		}
		// str += ","
		return str
	}
}
