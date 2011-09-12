#ifndef MACHINEGLWINDOW_H
#define MACHINEGLWINDOW_H

class IMachine;
class MachineThread;
#include <QGLWidget>
#include <QTime>

class HostVideo : public QGLWidget {
    Q_OBJECT
public:
	explicit HostVideo(IMachine *machine, MachineThread *thread);
	~HostVideo();

	bool isFpsVisible() const;
	void setFpsVisible(bool on);

	bool isPadVisible() const;
	void setPadVisible(bool on);

	bool isSwipeEnabled() const;
	void setSwipeEnabled(bool on);

	bool isQuickQuitVisible() const;
	void setQuickQuitVisible(bool on);

	void setMyVisible(bool visible);
signals:
	void wantClose();
	void minimized();
protected:
	void initializeGL();

	void paintEvent(QPaintEvent *);
	void closeEvent(QCloseEvent *e);
	void changeEvent(QEvent *e);
private slots:
	void updateRects();
private:
	IMachine *m_machine;
	MachineThread *m_thread;

	QRectF m_srcRect;
	QRectF m_dstRect;

	QString m_error;

	bool m_fpsVisible;
	int m_fpsCount;
	int m_fpsCounter;
	QTime m_fpsCounterTime;

	bool m_padVisible;
	QImage m_padArrowsImage;
	QImage m_padButtonsImage;
	QImage m_selectButtonImage;
	QImage m_startButtonImage;
	QImage m_pauseButtonImage;
	QImage m_quitButtonImage;

	bool m_swipeEnabled;
	bool m_quickQuitVisible;

	friend class MachineView;
};

inline bool HostVideo::isFpsVisible() const
{ return m_fpsVisible; }
inline bool HostVideo::isPadVisible() const
{ return m_padVisible; }
inline bool HostVideo::isSwipeEnabled() const
{ return m_swipeEnabled; }
inline bool HostVideo::isQuickQuitVisible() const
{ return m_quickQuitVisible; }

#endif // MACHINEGLWINDOW_H
