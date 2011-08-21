import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	Column {
		anchors.centerIn: parent
		spacing: 10

		Button {
			text: (audio.enable ? qsTr("Disable") : qsTr("Enable")) + qsTr(" Audio")
			onClicked: audio.enable = !audio.enable
		}
		Button {
			text: qsTr("Stereo")
			checkable: true
			checked: audio.stereoEnable
			onClicked: audio.stereoEnable = !audio.stereoEnable
			visible: audio.enable
		}
		Button {
			text: qsTr("SampleRate ") + audio.sampleRate
			visible: audio.enable
			// TODO dialog
		}
	}
}
