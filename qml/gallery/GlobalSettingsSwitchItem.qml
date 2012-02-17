import QtQuick 1.1
import com.nokia.meego 1.0
import "../base"

EMSwitchOption {
	property string optionName
	onCheckedChanged: diskGallery.setGlobalOption(optionName, checked)
	Component.onCompleted: checked = diskGallery.globalOption(optionName)
}
