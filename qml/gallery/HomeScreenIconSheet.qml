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

import QtQuick 1.0
import com.nokia.meego 1.0

Sheet {
	id: saveIconSheet
	property alias imgSource: img.source
	property alias imgScale: imageScaler.value

	property int iconX: 0
	property int iconY: 0

	// TODO waiting for new version of qt-components acceptButton.enabled when
	// icon selected -> rect.visible
	acceptButtonText: qsTr("Save")
	rejectButtonText: qsTr("Cancel")

	title: Label {
		anchors.centerIn: parent
		text: qsTr("Select Icon")
	}

	content: Item {
		anchors.fill: parent
		Label {
			width: parent.width
			text: qsTr("Info: Dialog creates an icon in the home screen.")
			wrapMode: Text.WordWrap
			horizontalAlignment: Text.AlignHCenter
		}
		Image {
			id: img
			anchors.centerIn: parent
			scale: imageScaler.value
		}
		Rectangle {
			id: rect
			x: img.x - (img.scale-1)*img.width/2 + saveIconSheet.iconX
			y: img.y - (img.scale-1)*img.height/2 + saveIconSheet.iconY
			width: 80
			height: 80
			color: Qt.rgba(1, 1, 1, 0.3)
			border.color: "white"
			border.width: 2
			visible: false
		}

		MouseArea {
			x: img.x - (img.scale-1)*img.width/2
			y: img.y - (img.scale-1)*img.height/2
			width: img.width*img.scale
			height: img.height*img.scale
			onClicked: {
				if (mouse.x >= 40 && mouse.y >= 40 && mouse.x+40 < width && mouse.y+40 < height) {
					saveIconSheet.iconX = mouse.x - 40
					saveIconSheet.iconY = mouse.y - 40
					rect.visible = true
				}
			}
		}
		Slider {
			id: imageScaler;
			anchors.right: parent.right
			height: parent.height
			orientation: Qt.Vertical
			minimumValue: 0.1; maximumValue: 2.0
			onValueChanged: rect.visible = false
		}
	}

	onAccepted: {
		if (!diskListModel.addIconToHomeScreen(currentDiskIndex, imageScaler.value, iconX, iconY)) {
			errorDialog.message = qsTr("Could not save the icon!")
			errorDialog.open()
		}
	}
}
