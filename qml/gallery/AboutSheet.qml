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

Sheet {
	id: aboutSheet

	acceptButtonText: ""
	rejectButtonText: "Close"

	title: Label {
		anchors.centerIn: parent
		text: "About"
	}
	content: Item {
		anchors.fill: parent
		Column {
			width: parent.width
			spacing: 50

			Label {
				width: parent.width
				text: "EmuMaster 0.0.2\nAuthor: Elemental\nWant new features? Found bug? Go to homepage"
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Image {
				source: "image://theme/icon-l-browser"
				anchors.horizontalCenter: parent.horizontalCenter

				MouseArea {
					anchors.fill: parent
					onClicked: romGallery.homepage()
				}
			}
			Label {
				width: parent.width
				text: "If you find this software useful please donate"
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Image {
				source: "../img/btn_donateCC_LG.png"
				scale: 2
				anchors.horizontalCenter: parent.horizontalCenter

				MouseArea {
					anchors.fill: parent
					onClicked: romGallery.donate()
				}
			}
		}
	}
}
