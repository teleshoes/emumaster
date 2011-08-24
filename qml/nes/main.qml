import QtQuick 1.1
import com.nokia.meego 1.0

PageStackWindow {
	property bool customStyleEnabled: true

	id: appWindow
	showStatusBar: false
	platformStyle: customStyle

	onCustomStyleEnabledChanged: {
		if (customStyleEnabled)
			platformStyle = customStyle
		else
			platformStyle = defaultStyle
	}

	PageStackWindowStyle {
		id: defaultStyle
	}
	PageStackWindowStyle {
		id: customStyle
		backgroundFillMode: Image.PreserveAspectCrop
		background: backgroundPath
	}

	initialPage: mainPage

	MainPage {
		id: mainPage
	}
	Component.onCompleted: {
		 screen.allowedOrientations = Screen.Landscape;
	}
}
