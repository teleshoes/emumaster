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
	Label {
		text: qsTr("TV Encoding System")
	}
	ButtonRow {
		id: tvSystemChooser
		anchors.horizontalCenter: parent.horizontalCenter
		checkedButton: tvSystemAuto
		spacing: 5

		Button { id: tvSystemAuto; text: qsTr("AUTO") }
		Button { id: tvSystemNtsc; text: qsTr("NTSC") }
		Button { id: tvSystemPal;  text: qsTr("PAL")  }
	}

	ListModel {
		id: renderMethodModel
		ListElement { name: QT_TR_NOOP("Auto") }
		ListElement { name: QT_TR_NOOP("Post All Render") }
		ListElement { name: QT_TR_NOOP("Pre All Render") }
		ListElement { name: QT_TR_NOOP("Post Render") }
		ListElement { name: QT_TR_NOOP("Pre Render") }
		ListElement { name: QT_TR_NOOP("Tile Render") }
	}

	SelectionItem {
		titleText: qsTr("Render Method")
		subtitleText: renderMethodModel[renderMethodDialog.selectedIndex]
		onClicked: renderMethodDialog.open()
	}

	SelectionDialog {
		id: renderMethodDialog
		model: renderMethodModel
		titleText: qsTr("Select Render Method")
		selectedIndex: 0
	}

	function confString() {
		var str = ""
		if (!tvSystemAuto.checked) {
			str += "nes.tvSystem="
			if (tvSystemNtsc.checked)
				str += "NTSC"
			else
				str += "PAL"
		}
		if (renderMethodDialog.selectedIndex > 0) {
			str += ","
			str += "nes.renderMethod="+(renderMethodDialog.selectedIndex-1)
		}
		// str += ","
		return str
	}
}
