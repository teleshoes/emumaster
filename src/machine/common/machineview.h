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
	explicit MachineView(QWidget *parent = 0);
	~MachineView();

	IMachine *machine() const;
	void setMachineAndQmlSettings(IMachine *m, const QString &qmlSettingsPath);

	void setSourceRect(const QRectF &rect);
	void setDestRect(const QRectF &rect);

	void showError(const QString &text);
public slots:
	void pause();
	void resume();
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

	friend class HostVideo;
	friend class MachineThread;
	friend class MachineImageProvider;
};

inline IMachine *MachineView::machine() const
{ return m_machine; }

#endif // MACHINEVIEW_H
