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

PageStackWindow {
	id: appWindow
	showStatusBar: false

	platformStyle: PageStackWindowStyle {
		id: customStyle
		background: "image://theme/meegotouch-video-background"
		backgroundFillMode: Image.Stretch
		cornersVisible: false
	}

	Component.onCompleted: {
		theme.inverted = true
		var qmlPage = emu.name.charAt(0).toUpperCase() +
				emu.name.substr(1) + "SettingsPage.qml"
		appWindow.pageStack.push(Qt.resolvedUrl(qmlPage))
	}
}
