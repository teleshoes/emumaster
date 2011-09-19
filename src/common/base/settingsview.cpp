#include "settingsview.h"
#include <QCloseEvent>

SettingsView::SettingsView() {
	setAttribute(Qt::WA_QuitOnClose, false);
}

void SettingsView::closeEvent(QCloseEvent *e) {
	e->ignore();
	emit wantClose();
}

void SettingsView::setMyVisible(bool visible) {
	if (visible) {
		showFullScreen();
		setFocus();
	} else {
		setVisible(false);
	}
}
