import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	tools: ToolBarLayout {
		ToolIcon { iconId: "toolbar-back"; onClicked: machineView.resume() }
		ButtonRow {
			platformStyle: TabButtonStyle { }
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-gallery"
				tab: videoTab
			}
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-content-audio"
				tab: audioTab
			}
			TabButton {
				iconSource: "../img/icon-m-toolbar-input.png"
				tab: inputTab
			}
			TabButton {
				iconSource: "../img/icon-m-toolbar-diskette.png"
				tab: stateTab
			}
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-frequent-used"
				tab: cheatTab
			}
		}
		ToolIcon { iconId: "toolbar-close"; onClicked: query.open() }
	}

	TabGroup {
		id: tabGroup
		currentTab: videoTab

		VideoPage { id: videoTab }
		AudioPage { id: audioTab }
		InputPage { id: inputTab }
		StatePage { id: stateTab }
		CheatPage { id: cheatTab }

		onCurrentTabChanged: appWindow.customStyleEnabled = (tabGroup.currentTab !== cheatTab)
	}

	QueryDialog {
		id: query
		icon: "image://theme/icon-m-common-red"
		titleText: "Really?"
		message: "Do you want to exit?"
		acceptButtonText: "Yes"
		rejectButtonText: "No"
		onAccepted: Qt.quit()
	}
}
