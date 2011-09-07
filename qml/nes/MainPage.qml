import QtQuick 1.1
import com.nokia.meego 1.0
import "../base"

Page {
	tools: ToolBarLayout {
		ToolIcon { iconId: "toolbar-back"; onClicked: machineView.resume() }
		ButtonRow {
			platformStyle: TabButtonStyle { }
			TabButton {
				iconSource: "image://theme/icon-m-toolbar-settings"
				tab: settingsTab
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
		currentTab: settingsTab

		SettingsPage { id: settingsTab }
		StatePage {
			id: stateTab

			Button {
				anchors.right: parent.right
				anchors.rightMargin: 10
				anchors.bottom: parent.bottom
				anchors.bottomMargin: 10
				width: 50
				iconSource: "image://theme/icon-s-music-video-description"
				onClicked: infoSheet.open()
			}
			MachineInfoSheet { id: infoSheet }
		}
		CheatPage { id: cheatTab }
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
