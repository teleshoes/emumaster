import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
	id: appWindow
	showStatusBar: false
	platformStyle: customStyle

	PageStackWindowStyle {
		id: customStyle
		backgroundFillMode: Image.PreserveAspectCrop
		background: backgroundPath
	}

	initialPage: mainPage

	MainPage { id: mainPage }
}
