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
