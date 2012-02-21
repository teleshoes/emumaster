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
	id: keybMappingPage

	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-back"
			onClicked: appWindow.pageStack.pop()
		}
	}
	Label {
		id: titleLabel
		text: qsTr("Touch Screen Settings")
		anchors.horizontalCenter: parent.horizontalCenter
	}

	Flickable {
		id: flickable
		anchors {
			top: titleLabel.bottom
			topMargin: 10
			bottom: parent.bottom
		}
		width: parent.width
		flickableDirection: Flickable.VerticalFlick
		clip: true
		contentHeight: items.height

		Column {
			id: items
			width: parent.width
			height: childrenRect.height
			spacing: 10


			Label { text: qsTr("Pad Opacity") }
			Slider {
				width: parent.width
				minimumValue: 0.0
				maximumValue: 1.0
				value: diskGallery.globalOption("padOpacity")
				onValueChanged: diskGallery.setGlobalOption("padOpacity", value)
				stepSize: 0.05
				valueIndicatorVisible: true
			}

			Label { text: qsTr("D-Pad Size") }
			Slider {
				id: dpadSizeSlider
				width: parent.width
				onValueChanged: diskGallery.setGlobalOption("touchAreaSize", value)
				stepSize: 2
				valueIndicatorVisible: true
				Component.onCompleted: {
					var initial = diskGallery.globalOption("touchAreaSize")
					minimumValue = 160
					maximumValue = 320
					value = initial
				}
			}

			Label { text: qsTr("D-Pad Diagonal Size") }
			Slider {
				width: parent.width
				maximumValue: dpadSizeSlider.value/2
				value: diskGallery.globalOption("touchAreaDiagonalSize")
				onValueChanged: diskGallery.setGlobalOption("touchAreaDiagonalSize", value)
				stepSize: 1
				valueIndicatorVisible: true
			}

			GlobalSettingsSwitchItem { text: qsTr("Haptic Feedback Enabled"); optionName: "hapticFeedbackEnable" }
			GlobalSettingsSwitchItem { text: qsTr("Show Buttons"); optionName: "buttonsVisible" }
			GlobalSettingsSwitchItem { text: qsTr("Show Grid"); optionName: "gridVisible" }
			GlobalSettingsSwitchItem { text: qsTr("Show L/R Buttons"); optionName: "lrButtonsVisible" }
		}
	}

	ScrollDecorator { flickableItem: flickable }
}
