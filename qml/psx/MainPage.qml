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
	tools: ToolBarLayout {
		ToolIcon { iconId: "toolbar-back"; onClicked: machineView.resume() }
		ButtonRow {
			platformStyle: TabButtonStyle { }
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-settings"
				tab: settingsTab
			}
			TabButton {
				iconSource: "../img/icon-m-toolbar-diskette.png"
				tab: stateTab
			}
			/*TabButton {
				iconSource: "image://theme/icon-m-toolbar-frequent-used"
				tab: cheatTab
			}*/
		}
		ToolIcon { iconId: "toolbar-close"; onClicked: query.open() }
	}

	TabGroup {
		id: tabGroup
		currentTab: settingsTab

		SettingsPage { id: settingsTab }
		StatePage { id: stateTab }
//		CheatPage { id: cheatTab }
	}

	QueryDialog {
		id: query
		icon: "image://theme/icon-m-common-red"
		titleText: "Really?"
		message: "Do you want to exit?"
		acceptButtonText: "Yes"
		rejectButtonText: "No"
		onAccepted: Qt.quit()
	}
}
