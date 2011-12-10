/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef MACHINEVIEW_H
#define MACHINEVIEW_H

class IMachine;
class MachineThread;
class HostVideo;
class HostAudio;
class HostInput;
class SettingsView;
class StateListModel;
#include "base_global.h"
#include <QGLWidget>
class QThread;
class QDeclarativeView;
class QSettings;

class BASE_EXPORT MachineView : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool fpsVisible READ isFpsVisible WRITE setFpsVisible NOTIFY fpsVisibleChanged)
	Q_PROPERTY(int frameSkip READ frameSkip WRITE setFrameSkip NOTIFY frameSkipChanged)
	Q_PROPERTY(bool audioEnable READ isAudioEnabled WRITE setAudioEnabled NOTIFY audioEnableChanged)
	Q_PROPERTY(qreal padOpacity READ padOpacity WRITE setPadOpacity NOTIFY padOpacityChanged)
	Q_PROPERTY(bool keepAspectRatio READ keepAspectRatio WRITE setKeepAspectRatio NOTIFY keepAspectRatioChanged)
	Q_PROPERTY(QString error READ error CONSTANT)
	Q_PROPERTY(QList<QObject *> inputDevices READ inputDevices NOTIFY inputDevicesChanged)
public:
	explicit MachineView(IMachine *machine, const QString &diskFileName);
	~MachineView();
	QDeclarativeView *settingsView() const;

	IMachine *machine() const;

	bool isRunning() const;

	bool isFpsVisible() const;
	void setFpsVisible(bool on);
	int frameSkip() const;
	void setFrameSkip(int n);
	bool isAudioEnabled() const;
	void setAudioEnabled(bool on);
	qreal padOpacity() const;
	void setPadOpacity(qreal opacity);
	bool keepAspectRatio() const;
	void setKeepAspectRatio(bool on);
	QList<QObject *> inputDevices() const;

	Q_INVOKABLE void saveScreenShot();

	QString error() const;
public slots:
	void pause();
	void resume();
	bool close();
signals:
	void fpsVisibleChanged();
	void frameSkipChanged();
	void audioEnableChanged();
	void padOpacityChanged();
	void keepAspectRatioChanged();
	void faultOccured(QString faultStr);
	void inputDevicesChanged();
private slots:
	void pauseStage2();
	void onFrameGenerated(bool videoOn);
	void onSlFailed();
	void onSafetyEvent();
	void onStateLoaded();
private:
	bool loadConfiguration();
	void loadSettings();
	QVariant loadOptionFromSettings(QSettings &s,
									const QString &name,
									const QVariant &defaultValue);
	QString extractArg(const QStringList &args, const QString &argName);
	void parseConfArg(const QString &arg);
	void setupSettingsView();
	void saveScreenShotIfNotExists();
	int determineLoadState(const QStringList &args);
	QString constructSlErrorString() const;
	void fatalError(const QString &faultStr);

	IMachine *m_machine;
	QString m_diskFileName;

	MachineThread *m_thread;
	HostInput *m_hostInput;
	HostAudio *m_hostAudio;
	HostVideo *m_hostVideo;
	StateListModel *m_stateListModel;
	SettingsView *m_settingsView;

	bool m_running;
	int m_backgroundCounter;
	bool m_quit;
	bool m_pauseRequested;
	int m_closeTries;

	bool m_audioEnable;
	bool m_autoSaveLoadEnable;
	QString m_error;

	bool m_safetyCheck;
	QTimer *m_safetyTimer;
};

inline IMachine *MachineView::machine() const
{ return m_machine; }
inline bool MachineView::isRunning() const
{ return m_running; }
inline QString MachineView::error() const
{ return m_error; }

#endif // MACHINEVIEW_H
