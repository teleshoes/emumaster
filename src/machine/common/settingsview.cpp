#include "settingsview.h"
#include "machineview.h"
#include <QCloseEvent>

SettingsView::SettingsView(MachineView *machineView) :
	m_machineView(machineView) {
}

void SettingsView::closeEvent(QCloseEvent *e) {
	e->ignore();
	m_machineView->close();
}

void SettingsView::setMyVisible(bool visible) {
	if (visible) {
#	if defined(MEEGO_EDITION_HARMATTAN)
		showFullScreen();
#	else
		resize(854, 480);
		setVisible(true);
#	endif
		setFocus();
	} else {
		setVisible(false);
	}
}

void SettingsView::changeEvent(QEvent *e) {
	if (e->type() == QEvent::WindowStateChange && !windowState().testFlag(Qt::WindowMinimized))
		emit updateOrientations();
	QDeclarativeView::changeEvent(e);
}
