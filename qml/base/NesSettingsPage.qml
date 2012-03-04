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
import com.nokia.meego 1.1

SettingsPage {
	inputContent: [
		SelectionItem {
			titleText: qsTr("NES Extra Input")
			subtitleText: extraInputModel.get(extraInputDialog.selectedIndex).name
			onClicked: extraInputDialog.open()
		}
	]

	videoContent: [
		EMSwitchOption {
			text: qsTr("Disable Sprite Limit")
			onCheckedChanged: emu.ppu.spriteLimit = !checked
			Component.onCompleted: checked = !emu.ppu.spriteLimit
		}
	]

	miscContent: [
		Label { width: parent.width; text: qsTr("Disk Info: ") + emu.diskInfo }
	]

	columnContent: [
		SectionSeperator { text: qsTr("ChEaTs") },
		NesCheats {}
	]

	SelectionDialog {
		id: extraInputDialog
		model: ListModel {
			id: extraInputModel
			ListElement { name: QT_TR_NOOP("None") }
			ListElement { name: QT_TR_NOOP("Zapper") }
			ListElement { name: QT_TR_NOOP("Paddle") }
		}
		titleText: qsTr("Select Extra Input")
		onSelectedIndexChanged: emu.pad.extraDevice = selectedIndex
		Component.onCompleted: selectedIndex = emu.pad.extraDevice
	}
}
