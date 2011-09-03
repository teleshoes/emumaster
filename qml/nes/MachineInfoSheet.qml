import QtQuick 1.0
import com.nokia.meego 1.0
import "../base/utils.js" as EmuMasterUtils

Sheet {
	id: infoSheet

	acceptButtonText: ""
	rejectButtonText: "Close"

	title: Label {
		anchors.centerIn: parent
		text: "Machine Info"
	}
	content: Flickable {
		anchors.fill: parent
		flickableDirection: Flickable.VerticalFlick
		Column {
			width: parent.width

			Label {
				width: parent.width
				text: "Disk ROM size: " + EmuMasterUtils.byteCountToText(machine.disk.romSize)
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Label {
				width: parent.width
				text: "Disk video ROM size: " + EmuMasterUtils.byteCountToText(machine.disk.vromSize)
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Label {
				width: parent.width
				text: "Disk RAM size: " + EmuMasterUtils.byteCountToText(machine.disk.ramSize)
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Label {
				width: parent.width
				text: "Disk CRC: " + EmuMasterUtils.hexToString(machine.disk.crc)
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Label {
				width: parent.width
				text: "Mapper: " + machine.mapper.name + " (" + machine.disk.mapperType + ")"
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
			Label {
				width: parent.width
				text: "Video System: " + (machine.disk.isPAL ? "PAL" : "NTSC" )
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
			}
		}
	}
}
