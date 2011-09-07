import QtQuick 1.1
import com.nokia.meego 1.0
import "../base"

Page {
	Flickable {
		id: flickable
		x: 15
		width: parent.width - 30
		height: parent.height
		contentWidth: width
		contentHeight: col.height
		flickableDirection: Flickable.VerticalFlick

		Column {
			id: col
			width: parent.width
			spacing: 15

			DefaultSettings {
				width: parent.width
			}
			EMButtonOption {
				labelText: qsTr("PPU Render Method")
				buttonText: renderMethodModel.get(machine.ppu.renderMethod)["name"]
				onClicked: renderMethodDialog.open()
			}
			EMSwitchOption {
				text: qsTr("PPU Sprite Clipping")
				checked: machine.ppu.spriteClippingEnable; onCheckedChanged: machine.ppu.spriteClippingEnable = checked
			}
		}
	}
	ListModel {
		id: renderMethodModel
		ListElement { name: "Post All Render" }
		ListElement { name: "Pre All Render" }
		ListElement { name: "Post Render" }
		ListElement { name: "Pre Render" }
		ListElement { name: "Tile Render" }
	}
	SelectionDialog {
		id: renderMethodDialog
		titleText: "Select Render Method"
		model: renderMethodModel
		onAccepted: machine.ppu.renderMethod = selectedIndex
	}
}
