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
	CoverFlow {
		id: coverFlow
		anchors.fill: parent
		model: romListModel
		delegate: CoverFlowDelegate {
			imageSource: "image://rom/" + romListModel.machineName + "_" + title + "*" + screenShotUpdate

			Label {
				anchors.top: parent.top
				anchors.topMargin: -30
				anchors.horizontalCenter: parent.horizontalCenter
				text: title
				visible: romChooserPage.currentRomIndex == index
			}
		}
		pathItemCount: 7
		currentIndex: romChooserPage.currentRomIndex

		onCurrentIndexChanged: {
			if (coverFlowEnabled)
				romChooserPage.currentRomIndex = currentIndex
		}

		Connections {
			target: romChooserPage

			onCurrentRomIndexChanged: {
				if (!coverFlowEnabled)
					coverFlow.currentIndex = romChooserPage.currentRomIndex
			}
		}
	}
}
