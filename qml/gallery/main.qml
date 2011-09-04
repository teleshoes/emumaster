import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
	id: appWindow
	showStatusBar: inPortrait

	initialPage: mainPage

	RomChoosePage {
		id: mainPage
	}
}
