#ifndef MACHINEGLWINDOW_H
#define MACHINEGLWINDOW_H

class MachineView;
class MachineThread;
class HostInput;
#include <QGLWidget>
#include <QTime>

class HostVideo : public QGLWidget {
    Q_OBJECT
	Q_PROPERTY(bool fpsVisible READ isFpsVisible WRITE setFpsVisible NOTIFY fpsVisibleChanged)
	Q_PROPERTY(int frameSkip READ frameSkip WRITE setFrameSkip NOTIFY frameSkipChanged)
public:
	explicit HostVideo(MachineView *);
	~HostVideo();

	bool isFpsVisible() const;
	void setFpsVisible(bool on);

	int frameSkip() const;
	void setFrameSkip(int skip);
	QImage screenShotGrayscaled() const;
	void setMyVisible(bool visible);
	void setupSwipe(bool on);
signals:
	void fpsVisibleChanged();
	void frameSkipChanged();
protected:
	void initializeGL();

	void paintEvent(QPaintEvent *);
	void closeEvent(QCloseEvent *e);
	void changeEvent(QEvent *e);
private:
	QRectF m_srcRect;
	QRectF m_dstRect;

	QString m_error;

	bool m_fpsVisble;
	int m_fpsCount;
	int m_fpsCounter;
	QTime m_fpsCounterTime;

	int m_frameSkip;
	MachineThread *m_thread;
	MachineView *m_machineView;
	HostInput *m_hostInput;
	QImage m_padArrowsImage;
	QImage m_padButtonsImage;
	QImage m_selectButtonImage;
	QImage m_startButtonImage;
	QImage m_pauseButtonImage;

	friend class MachineView;
};

inline bool HostVideo::isFpsVisible() const
{ return m_fpsVisble; }
inline int HostVideo::frameSkip() const
{ return m_frameSkip; }

#endif // MACHINEGLWINDOW_H
