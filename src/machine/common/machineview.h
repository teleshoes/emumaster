#ifndef MACHINEVIEW_H
#define MACHINEVIEW_H

class IMachine;
class MachineThread;
class HostVideo;
class HostAudio;
class HostInput;
class SettingsView;
class MachineImageProvider;
class MachineStateListModel;
class GameGenieCodeListModel;
#include "machine_common_global.h"
#include <QGLWidget>
class QThread;
class QDeclarativeView;

class MACHINE_COMMON_EXPORT MachineView : public QObject {
	Q_OBJECT
public:
	explicit MachineView(IMachine *machine, const QString &diskName, QWidget *parent = 0);
	~MachineView();

	// TODO move to IMachine
	static QString romDirPath();
	static QString userDataDirPath();
	static void buildLocalDirTree();

	IMachine *machine() const;

	void showError(const QString &text);
	bool isRunning() const;

	Q_INVOKABLE void saveScreenShot();
	Q_INVOKABLE bool isGameGenieCodeValid(const QString &s);
public slots:
	void pause();
	void resume();
	bool close();
private slots:
	void pauseStage2();
private:
	void saveSettings();
	void loadSettings();

	QString screenShotPath() const;

	SettingsView *m_settingsView;
	HostVideo *m_hostVideo;
	HostAudio *m_hostAudio;
	HostInput *m_hostInput;
	MachineThread *m_thread;
	IMachine *m_machine;
	bool m_running;
	int m_backgroundCounter;
	QString m_diskName;
	bool m_wantClose;

	MachineStateListModel *m_stateListModel;
	bool m_autoLoadOnStart;
	bool m_autoSaveOnExit;

	GameGenieCodeListModel *m_gameGenieCodeListModel;

	friend class HostVideo;
	friend class MachineThread;
	friend class MachineImageProvider;
};

inline IMachine *MachineView::machine() const
{ return m_machine; }

inline bool MachineView::isRunning() const
{ return m_running; }

#endif // MACHINEVIEW_H
