import QtQuick 1.1

Item {
	id: button
	property string name
	width: 240; height: 240
	Image {
		anchors.centerIn: parent
		source: "../img/machine-" + name + ".png"
	}
	Text {
		text: button.name
		anchors.horizontalCenter: button.horizontalCenter
		anchors.top: button.top
		anchors.topMargin: 30
		font.pointSize: 20
		font.bold: true
		font.capitalization: Font.AllUppercase
		color: "black"
	}
	BorderImage {
		id: overlay
		anchors.fill: parent
		source: "image://theme/meegotouch-panel-background-selected"
		opacity: 0.5
		visible: romListModel.machineName === button.name
	}
	MouseArea {
		anchors.fill: parent
		onClicked: romChooserPage.setMachineName(button.name)
	}
}
