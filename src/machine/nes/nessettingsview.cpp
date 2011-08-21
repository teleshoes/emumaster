#include "nessettingsview.h"

NesSettingsView::NesSettingsView(QWidget *parent) :
    QDeclarativeView(parent)
{
	setSource(QUrl::fromLocalFile("../qml/nes/main.qml"));
}
