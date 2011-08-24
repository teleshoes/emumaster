import QtQuick 1.1
import com.nokia.meego 1.0
import "../common"

Page {
	CoverFlow {
		id: coverFlow
		anchors.fill: parent
		model: romListModel
		delegate: CoverFlowDelegate {
			imageSource: "image://rom/" + romListModel.machineName + title + "*" + screenShotUpdate

			Label {
				anchors.top: parent.top
				anchors.topMargin: -30
				anchors.horizontalCenter: parent.horizontalCenter
				text: title
				visible: romChooserPage.currentRomIndex == index
			}
		}
		pathItemCount: 7
		currentIndex: romChooserPage.currentRomIndex

		onCurrentIndexChanged: {
			if (coverFlowEnabled)
				romChooserPage.currentRomIndex = currentIndex
		}

		Connections {
			target: romChooserPage

			onCurrentRomIndexChanged: {
				if (!coverFlowEnabled)
					coverFlow.currentIndex = romChooserPage.currentRomIndex
			}
		}
	}
}
