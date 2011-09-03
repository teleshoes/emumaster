#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QDeclarativeView>

class SettingsView : public QDeclarativeView {
    Q_OBJECT
public:
	SettingsView();
	void setMyVisible(bool visible);
signals:
	void wantClose();
protected:
	void closeEvent(QCloseEvent *e);
};

#endif // SETTINGSVIEW_H
