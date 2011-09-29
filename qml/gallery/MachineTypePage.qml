import QtQuick 1.1
import com.nokia.meego 1.0

Page {
	Grid {
		anchors.centerIn: parent
		rows: 2
		columns: 2

		MachineTypeButton { name: "nes" }
		MachineTypeButton { name: "snes" }
		MachineTypeButton { name: "gba" }
		MachineTypeButton { name: "psx"; alphaVersion: true }
	}
}
