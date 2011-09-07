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
				height: childrenRect.height + parent.spacing
			}
		}
	}
}
