/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

import QtQuick 1.1
import com.nokia.meego 1.0

Column {
	property alias labelText: label.text
	property alias buttonText: button.text
	signal clicked

	id: item
	width: parent.width
	spacing: 5

	Label {
		id: label
		font.bold: true
	}
	Button {
		id: button
		anchors.horizontalCenter: parent.horizontalCenter
		onClicked: item.clicked()
	}
}
