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
	ListModel { id: nullModel }
	function update() {
		listView.model = nullModel
		listView.model = diskListModel
	}

	ListView {
		id: listView
		anchors.fill: parent
		currentIndex: diskChooserPage.currentDiskIndex

		delegate: MyListDelegate {
			id: delegate

			BorderImage {
				id: overlay
				anchors.fill: parent
				source: "image://theme/meegotouch-panel-background-selected"
				opacity: 0.5
				visible: false
			}

			onClicked: diskChooserPage.currentDiskIndex = index

			states: [
				State {
					name: "selected"; when: listView.currentIndex === index
					PropertyChanges { target: overlay; visible: true }
				}
			]
		}

		section.property: "alphabet"
		section.criteria: ViewSection.FullString
		section.delegate: Item {
			width: parent.width
			height: 40
			Text {
				id: headerLabel
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				anchors.rightMargin: 8
				anchors.bottomMargin: 2
				text: section
				font.bold: true
				font.pointSize: 18
				color: theme.inverted ? "#4D4D4D" : "#3C3C3C";
			}
			Image {
				anchors.right: headerLabel.left
				anchors.left: parent.left
				anchors.verticalCenter: headerLabel.verticalCenter
				anchors.rightMargin: 24
				source: "image://theme/meegotouch-groupheader" + (theme.inverted ? "-inverted" : "") + "-background"
			}
		}
	}
	MySectionScroller {
		id: sectionScroller
		listView: listView
	}
	ScrollDecorator {
		flickableItem: listView
	}
}
