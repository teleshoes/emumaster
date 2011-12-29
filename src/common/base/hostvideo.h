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

#ifndef MACHINEGLWINDOW_H
#define MACHINEGLWINDOW_H

class IMachine;
class MachineThread;
class HostInput;
#include <QGLWidget>
#include <QTime>

class HostVideo : public QGLWidget {
    Q_OBJECT
public:
#if defined(MEEGO_EDITION_HARMATTAN)
	static const int Width = 854.0f;
	static const int Height = 480.0f;
#elif defined(Q_WS_MAEMO_5)
	static const int Width = 800.0f;
	static const int Height = 480.0f;
#endif

	explicit HostVideo(HostInput *hostInput, IMachine *machine, MachineThread *thread);
	~HostVideo();

	bool isFpsVisible() const;
	void setFpsVisible(bool on);

	bool isSwipeEnabled() const;
	void setSwipeEnabled(bool on);

	bool keepApsectRatio() const;
	void setKeepAspectRatio(bool on);

	bool bilinearFiltering() const;
	void setBilinearFiltering(bool enabled);

	void setMyVisible(bool visible);

	QRectF dstRect() const;
signals:
	void quit();
	void minimized();
protected:
	void paintEvent(QPaintEvent *);
	void closeEvent(QCloseEvent *e);
	void changeEvent(QEvent *e);
private slots:
	void updateRects();
private:
	void paintFps(QPainter &painter);

	HostInput *m_hostInput;
	IMachine *m_machine;
	MachineThread *m_thread;

	QRectF m_srcRect;
	QRectF m_dstRect;

	bool m_fpsVisible;
	int m_fpsCount;
	int m_fpsCounter;
	QTime m_fpsCounterTime;

	bool m_swipeEnabled;
	bool m_keepAspectRatio;
	bool m_bilinearFiltering;
	bool m_firstPaint;
};

inline QRectF HostVideo::dstRect() const
{ return m_dstRect; }
inline bool HostVideo::isFpsVisible() const
{ return m_fpsVisible; }
inline bool HostVideo::isSwipeEnabled() const
{ return m_swipeEnabled; }
inline bool HostVideo::keepApsectRatio() const
{ return m_keepAspectRatio; }
inline bool HostVideo::bilinearFiltering() const
{ return m_bilinearFiltering; }

#endif // MACHINEGLWINDOW_H
