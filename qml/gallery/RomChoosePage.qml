import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	id: romChooserPage
	property int currentRomIndex: -1
	property bool coverFlowEnabled: true

	tools: ToolBarLayout {
//	TODO console choose	ToolIcon { iconId: "toolbar-back"; onClicked:  }
		ButtonRow {
			platformStyle: TabButtonStyle { }
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-gallery"
				tab: galleryTab
			}
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-list"
				tab: listTab
			}
		}
		// TODO about
		ToolIcon {
			iconId: "toolbar-home"
			visible: currentRomIndex >= 0
			// TODO add icon page
		}
		ToolIcon {
			iconId: "toolbar-mediacontrol-play"
			visible: currentRomIndex >= 0
			onClicked: romGallery.launch(romListModel.machineName, romListModel.get(currentRomIndex) )
		}
	}

	TabGroup {
		id: tabGroup
		currentTab: galleryTab

		GalleryPage { id: galleryTab }
		ListPage { id: listTab }

		onCurrentTabChanged: coverFlowEnabled = (currentTab === galleryTab)
	}
	Component.onCompleted: {
		romListModel.machineName = "nes"
	}
}
