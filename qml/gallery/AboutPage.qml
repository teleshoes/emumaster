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

Page {
	id: aboutPage
	orientationLock: PageOrientation.LockPortrait

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-back"
			onClicked: appWindow.pageStack.pop()
		}
	}

	Column {
		width: parent.width
		spacing: 50

		Label {
			width: parent.width
			text: qsTr("EmuMaster %1\nAuthor: elemental\n" +
					   "Want new features?\nFound a bug?\nVisit the homepage or the wiki")
						.arg(appVersion)
			wrapMode: Text.WordWrap
			horizontalAlignment: Text.AlignHCenter
		}
		Row {
			anchors.horizontalCenter: parent.horizontalCenter
			spacing: 100

			Column {
				Image {
					id: iconHome
					source: "image://theme/icon-l-browser"
					MouseArea {
						anchors.fill: parent
						onClicked: diskGallery.homepage()
					}
				}
				Label {
					anchors.horizontalCenter: iconHome.horizontalCenter
					text: qsTr("Home")
				}
			}
			Column {
				Image {
					id: iconWiki
					source: "../img/wiki.png"
					width: 80; height: 80
					MouseArea {
						anchors.fill: parent
						onClicked: diskGallery.wiki()
					}
				}
				Label {
					anchors.horizontalCenter: iconWiki.horizontalCenter
					text: qsTr("Wiki")
				}
			}
		}
		Label {
			width: parent.width
			text: qsTr("If you find this software useful, please donate")
			wrapMode: Text.WordWrap
			horizontalAlignment: Text.AlignHCenter
		}
		Image {
			source: "../img/btn_donateCC_LG.png"
			scale: 2
			anchors.horizontalCenter: parent.horizontalCenter

			MouseArea {
				anchors.fill: parent
				onClicked: diskGallery.donate()
			}
		}
	}
}
