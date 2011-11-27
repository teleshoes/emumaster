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
	id: mainPage
	orientationLock: PageOrientation.LockPortrait

	Label {
		id: helpLabel
		anchors.horizontalCenter: parent.horizontalCenter
		y: 10
		text: qsTr("Visit wiki before using it !!!")
	}

	Button {
		id: startButton
		anchors {
			horizontalCenter: parent.horizontalCenter
			top: helpLabel.bottom
			topMargin: 10
		}
		text: qsTr("Start Monitor")
		onClicked: {
			var err = sixAxisMonitor.start()
			if (err === "") {
				helpLabel.text = qsTr("Monitor Running")
				startButton.visible = false
			} else {
				errDialog.message = qsTr("Something went wrong: ") + err
			}
		}
	}

	ListView {
		id: sixAxisListView
		width: parent.width
		anchors {
			top: startButton.bottom
			topMargin: 10
			bottom: parent.bottom
		}
		spacing: 10
		model: sixAxisMonitor.addresses
		delegate: Column {
			anchors.horizontalCenter: parent.horizontalCenter
			width: childrenRect.width
			spacing: 4
			Label {
				id: infoLabel
				text: qsTr("%1. Bluetooth address: %2")
							.arg(index+1)
							.arg(modelData)
			}
			ButtonRow {
				exclusive: false
				Button {
					text: qsTr("Identify")
					onClicked: sixAxisMonitor.identify(index)
				}
				Button {
					text: qsTr("Disconnect")
					onClicked: sixAxisMonitor.disconnectDev(index)
				}
			}
		}
	}

	QueryDialog {
		id: errDialog
		rejectButtonText: qsTr("Close")
		titleText: qsTr("Error")
	}
}
