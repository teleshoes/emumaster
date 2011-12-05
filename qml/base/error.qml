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

PageStackWindow {
	id: appWindow
	showStatusBar: false

	style: PageStackWindowStyle {
		id: customStyle
		background: "image://theme/meegotouch-video-background"
		backgroundFillMode: Image.Stretch
	}

	initialPage: Page {
		Timer {
			interval: 1
			repeat: false
			running: true
			triggeredOnStart: false
			onTriggered: errorDialog.open()
		}

		QueryDialog {
			id: errorDialog
			titleText: qsTr("Error")
			message: machineView.error + qsTr("\n Application is going to shutdown :(")
			rejectButtonText: qsTr("Close")
			onRejected: Qt.quit()
		}
	}

	Component.onCompleted: theme.inverted = true
}
