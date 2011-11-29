import QtQuick 1.1
import com.nokia.meego 1.0
import "../base"

Page {
	orientationLock: PageOrientation.LockPortrait
	tools: ToolBarLayout {
		ToolIcon {
			iconId: "toolbar-back"
			onClicked: appWindow.pageStack.pop()
		}
	}

	Flickable {
		anchors.fill: parent
		flickableDirection: Flickable.VerticalFlick

	Column {
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
			id: padOpacityLabel
			text: qsTr("Pad Opacity")
			font.bold: true
		}
		Slider {
			id: padOpacitySlider
			width: parent.width
			minimumValue: 0.0
			maximumValue: 1.0
			value: diskGallery.globalOption("padOpacity", 0.45)
			onValueChanged: diskGallery.setGlobalOption("padOpacity", value)
			stepSize: 0.05
			valueIndicatorVisible: true
		}

		SectionSeperator { text: qsTr("VIDEO"); rightPad: 150 }
		Label {
			id: frameSkipLabel
			text: qsTr("Frameskip")
			font.bold: true
		}
		Slider {
			id: frameSkipSlider
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
			onCheckedChanged: diskGallery.setlobalOption("keepAspectRatio", !checked)
		}
	}

	}
}
