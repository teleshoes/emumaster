#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

class MachineView;
#include <QDeclarativeView>

class SettingsView : public QDeclarativeView {
    Q_OBJECT
public:
	explicit SettingsView(MachineView *machineView);
	void setMyVisible(bool visible);
signals:
	void updateOrientations();
protected:
	void closeEvent(QCloseEvent *e);
	void changeEvent(QEvent *e);
private:
	MachineView *m_machineView;
};

#endif // SETTINGSVIEW_H
