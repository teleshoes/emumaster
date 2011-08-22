#ifndef MACHINEVIEW_H
#define MACHINEVIEW_H

class IMachine;
class MachineThread;
class HostVideo;
class HostAudio;
class HostInput;
class MachineImageProvider;
#include "machine_common_global.h"
#include <QGLWidget>
class QThread;
class QDeclarativeView;

class MACHINE_COMMON_EXPORT MachineView : public QWidget {
	Q_OBJECT
public:
	explicit MachineView(IMachine *machine, const QString &diskName, QWidget *parent = 0);
	~MachineView();

	IMachine *machine() const;

	void showError(const QString &text);
	bool isRunning() const;

	Q_INVOKABLE void saveScreenShot();
public slots:
	void pause();
	void resume();
signals:
	void runningChanged();
private slots:
	void pauseStage2();
protected:
	void closeEvent(QCloseEvent *);
private:
	void setupSwipe(bool on);

	QDeclarativeView *m_settingsView;
	HostVideo *m_hostVideo;
	HostAudio *m_hostAudio;
	HostInput *m_hostInput;
	MachineThread *m_thread;
	IMachine *m_machine;
	bool m_running;
	int m_backgroundCounter;
	QString m_diskName;
	bool m_wantClose;

	friend class HostVideo;
	friend class MachineThread;
	friend class MachineImageProvider;
};

inline IMachine *MachineView::machine() const
{ return m_machine; }

inline bool MachineView::isRunning() const
{ return m_running; }

#endif // MACHINEVIEW_H
