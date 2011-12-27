import QtQuick 1.1
import com.nokia.meego 1.0
import "../base"

Page {
	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-back"
			onClicked: appWindow.pageStack.pop()
		}
	}

	Flickable {
		id: flickable
		anchors.fill: parent
		flickableDirection: Flickable.VerticalFlick
		contentHeight: column.height

	Column {
		id: column
		width: parent.width
		height: childrenRect.height
		spacing: 20

		SectionSeperator { text: qsTr("STATE"); rightPad: 150 }
		EMSwitchOption {
			text: qsTr("Auto Load/Save on Start/Exit")
			checked: diskGallery.globalOption("autoSaveLoadEnable", true)
			onCheckedChanged: diskGallery.setGlobalOption("autoSaveLoadEnable", checked)
		}

		SectionSeperator { text: qsTr("INPUT"); rightPad: 150 }
		EMSwitchOption {
			text: qsTr("Swipe Enabled")
			checked: diskGallery.globalOption("swipeEnable", false)
			onCheckedChanged: diskGallery.setGlobalOption("swipeEnable", checked)
		}
		Label {
			text: qsTr("Pad Opacity")
			font.bold: true
		}
		Slider {
			width: parent.width
			minimumValue: 0.0
			maximumValue: 1.0
			value: diskGallery.globalOption("padOpacity", 0.45)
			onValueChanged: diskGallery.setGlobalOption("padOpacity", value)
			stepSize: 0.05
			valueIndicatorVisible: true
		}
		Button {
			text: qsTr("Keyboard Configuration")
			anchors.horizontalCenter: parent.horizontalCenter
			onClicked: appWindow.pageStack.push(Qt.resolvedUrl("KeybMappingPage.qml"))
		}
		Button {
			text: qsTr("Calibrate Accelerometer")
			anchors.horizontalCenter: parent.horizontalCenter
			onClicked: appWindow.pageStack.push(Qt.resolvedUrl("AccelCalibrationPage.qml"))
		}

		SectionSeperator { text: qsTr("VIDEO"); rightPad: 150 }
		Label {
			text: qsTr("Frameskip")
			font.bold: true
		}
		Slider {
			width: parent.width
			minimumValue: 0
			maximumValue: 5
			value: diskGallery.globalOption("frameSkip", 1)
			onValueChanged: diskGallery.setGlobalOption("frameSkip", value)
			stepSize: 1
			valueIndicatorVisible: true
		}
		EMSwitchOption {
			text: qsTr("Show FPS")
			checked: diskGallery.globalOption("fpsVisible", false)
			onCheckedChanged: diskGallery.setGlobalOption("fpsVisible", checked)
		}
		EMSwitchOption {
			text: qsTr("Force Fullscreen")
			checked: !diskGallery.globalOption("keepAspectRatio", true)
			onCheckedChanged: diskGallery.setGlobalOption("keepAspectRatio", !checked)
		}
		EMSwitchOption {
			id: runInBackgroundSwitch
			text: qsTr("Run in Background")
			checked: diskGallery.globalOption("runInBackground", false)
			onCheckedChanged: {
				if (checked) {
					// special handling to prevent the dialog from showing at the start
					if (diskGallery.globalOption("runInBackground", false) !== "true")
						runInBackgroundDialog.open()
				} else {
					diskGallery.setGlobalOption("runInBackground", false)
				}
			}
		}
	}

	}
	ScrollDecorator { flickableItem: flickable }

	QueryDialog {
		id: runInBackgroundDialog
		titleText: qsTr("Enabling the option will run the application in the background. " +
						"This can consume a lot of battery!")
		acceptButtonText: qsTr("Enable")
		rejectButtonText: qsTr("Disable")
		onRejected: runInBackgroundSwitch.checked = false
		onAccepted: diskGallery.setGlobalOption("runInBackground", true)
	}
}
