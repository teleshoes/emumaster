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
#include <QGLWidget>
#include <QTime>

class HostVideo : public QGLWidget {
    Q_OBJECT
public:
	explicit HostVideo(IMachine *machine, MachineThread *thread);
	~HostVideo();

	bool isFpsVisible() const;
	void setFpsVisible(bool on);

	qreal padOpacity() const;
	void setPadOpacity(qreal opacity);

	bool isSwipeEnabled() const;
	void setSwipeEnabled(bool on);

	bool keepApsectRatio() const;
	void setKeepAspectRatio(bool on);

	void setMyVisible(bool visible);

	QRectF dstRect() const;
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

	bool m_fpsVisible;
	int m_fpsCount;
	int m_fpsCounter;
	QTime m_fpsCounterTime;

	qreal m_padOpacity;
	QImage m_padLeftImage;
	QImage m_padRightImage;

	bool m_swipeEnabled;
	bool m_keepAspectRatio;
};

inline QRectF HostVideo::dstRect() const
{ return m_dstRect; }
inline bool HostVideo::isFpsVisible() const
{ return m_fpsVisible; }
inline qreal HostVideo::padOpacity() const
{ return m_padOpacity; }
inline bool HostVideo::isSwipeEnabled() const
{ return m_swipeEnabled; }
inline bool HostVideo::keepApsectRatio() const
{ return m_keepAspectRatio; }

#endif // MACHINEGLWINDOW_H
