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
#include "base_global.h"
#include <QGLWidget>
class QThread;
class QDeclarativeView;

class BASE_EXPORT MachineView : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool fpsVisible READ isFpsVisible WRITE setFpsVisible NOTIFY fpsVisibleChanged)
	Q_PROPERTY(int frameSkip READ frameSkip WRITE setFrameSkip NOTIFY frameSkipChanged)
	Q_PROPERTY(bool audioEnable READ isAudioEnabled WRITE setAudioEnabled NOTIFY audioEnableChanged)
	Q_PROPERTY(int audioSampleRate READ audioSampleRate WRITE setAudioSampleRate NOTIFY audioSampleRateChanged)
	Q_PROPERTY(bool swipeEnable READ isSwipeEnabled WRITE setSwipeEnabled NOTIFY swipeEnableChanged)
	Q_PROPERTY(qreal padOpacity READ padOpacity WRITE setPadOpacity NOTIFY padOpacityChanged)
	Q_PROPERTY(bool keepAspectRatio READ keepAspectRatio WRITE setKeepAspectRatio NOTIFY keepAspectRatioChanged)
	Q_PROPERTY(bool accelerometerEnable READ isAccelerometerEnabled WRITE setAccelerometerEnabled NOTIFY accelerometerEnableChanged)
public:
	explicit MachineView(IMachine *machine, const QString &diskName);
	~MachineView();

	IMachine *machine() const;

	void showError(const QString &text);
	bool isRunning() const;

	bool isFpsVisible() const;
	void setFpsVisible(bool on);
	int frameSkip() const;
	void setFrameSkip(int n);
	bool isAudioEnabled() const;
	void setAudioEnabled(bool on);
	int audioSampleRate() const;
	void setAudioSampleRate(int rate);
	bool isSwipeEnabled() const;
	void setSwipeEnabled(bool on);
	qreal padOpacity() const;
	void setPadOpacity(qreal opacity);
	bool keepAspectRatio() const;
	void setKeepAspectRatio(bool on);
	bool isAccelerometerEnabled() const;
	void setAccelerometerEnabled(bool on);

	Q_INVOKABLE void saveScreenShot();

	QDeclarativeView *settingsView() const;
public slots:
	void pause();
	void resume();
	bool close();
signals:
	void fpsVisibleChanged();
	void frameSkipChanged();
	void audioEnableChanged();
	void audioSampleRateChanged();
	void swipeEnableChanged();
	void padOpacityChanged();
	void keepAspectRatioChanged();
	void accelerometerEnableChanged();
private slots:
	void pauseStage2();
	void onFrameGenerated(bool videoOn);
private:
	void saveSettings();
	void loadSettings();

	IMachine *m_machine;
	QString m_diskName;

	MachineThread *m_thread;
	HostInput *m_hostInput;
	HostAudio *m_hostAudio;
	HostVideo *m_hostVideo;
	MachineStateListModel *m_stateListModel;
	SettingsView *m_settingsView;

	bool m_running;
	int m_backgroundCounter;
	bool m_wantClose;
	bool m_pauseRequested;

	bool m_autoLoadOnStart;
	bool m_autoSaveOnExit;
	bool m_audioEnable;
	int m_audioSampleRate;
};

inline IMachine *MachineView::machine() const
{ return m_machine; }
inline bool MachineView::isRunning() const
{ return m_running; }

#endif // MACHINEVIEW_H
