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

PageStackWindow {
	id: appWindow
	initialPage: mainPage

	style: PageStackWindowStyle {
		id: customStyle;
		background: "image://theme/meegotouch-video-background"
		backgroundFillMode: Image.Stretch
	}
	CollectionMenuPage { id: mainPage }
	GalleryPage { id: galleryPage }

	function selectCollection(name) {
		diskListModel.collection = name
		if (diskListModel.count <= 0) {
			if (name == "fav")
				howToAddToFavDialog.open()
			else
				howToInstallDiskDialog.open()
		} else {
			appWindow.pageStack.push(galleryPage)
		}
		galleryPage.updateModel()
	}

	QueryDialog {
		id: howToInstallDiskDialog
		rejectButtonText: qsTr("Close")
		titleText: qsTr("Help")
		message: qsTr("The directory is empty. To install a disk you need to attach " +
					  "the phone to the PC and copy your files to \"emumaster/%1\" " +
					  "directory. Remember to detach the phone from the PC.")
						.arg(diskListModel.collection)
	}

	QueryDialog {
		id: howToAddToFavDialog
		rejectButtonText: qsTr("Close")
		titleText: qsTr("Help")
		message: qsTr("The favourite category is empty. To add a disk to favourite list " +
					  "just select the disk from one of emulated systems (press and hold), " +
					  "a menu will appear, choose \"Add To Favourites\"")
	}

	QueryDialog {
		id: firstRunMsg
		titleText: qsTr("Info")
		rejectButtonText: qsTr("Close")
		message:	qsTr("Hi! Few clues: Please let me know about any problems " +
						"before you make a review in the Store since I cannot " +
						"answer you there. You can find wiki address and contact info at the about " +
						"page:\n MainWindow->Menu->About EmuMaster...\n Enjoy :)\n " +
						"Have you got some idea? Share it with me.\n" +
						"Consider a donation if you find this software useful.") +
						(diskGallery.runCount < 10
							?	qsTr("\n\nThis message will be shown %1 more times")
									.arg(10-diskGallery.runCount)
							: "")

	}

	QueryDialog {
		id: detachUsbDialog
		message: qsTr(	"\"emumaster\" folder not found! Detach USB cable if " +
						"connected and restart the application."	)
		rejectButtonText: qsTr("Close")
		onRejected: Qt.quit()
	}

	Connections {
		target: diskGallery
		//onDiskUpdate: selectCollection(diskListModel.collectionLastUsed)
		onDetachUsb: detachUsbDialog.open()
		onShowFirstRunMsg: firstRunMsg.open()
	}

	QueryDialog {
		id: errorDialog
		titleText: qsTr("Oops")
		message: qsTr("Something went wrong!")
		rejectButtonText: qsTr("Close")
	}

	Component.onCompleted: {
		theme.inverted = true
	}

	property Item keysForwardedTo: mainPage

	Connections {
		target: pageStack
		onCurrentPageChanged: keysForwardedTo = pageStack.currentPage
	}
	Keys.forwardTo: keysForwardedTo
}
