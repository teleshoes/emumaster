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

Item {
	id: imageListViewDelegate
	width: parent.width
	height: 280

	property alias imgSource: screenShot.source
	property alias text: itemLabel.text
	signal clicked
	signal pressAndHold

	Image {
		id: screenShot
		x: 10
		width: parent.width-20
		height: parent.height-30
	}
	Label {
		id: itemLabel
		x: 10
		anchors.top: screenShot.bottom
		anchors.topMargin: 8
		font.bold: true
		color: "white"
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		onClicked: imageListViewDelegate.clicked()
		onPressAndHold: imageListViewDelegate.pressAndHold()
	}
	Behavior on scale {
		NumberAnimation { duration: 100 }
	}

	states: [
		State {
			name: "pressed"
			when: mouseArea.pressed
			PropertyChanges {
				target: imageListViewDelegate
				scale: 0.85
			}
		}
	]
}
