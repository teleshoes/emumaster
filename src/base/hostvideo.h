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

#ifndef HOSTVIDEO_H
#define HOSTVIDEO_H

class Emu;
class EmuThread;
class HostInput;
#include "base_global.h"
#include <QGLWidget>
#include <QTime>
#include <QGLShaderProgram>

class BASE_EXPORT HostVideo : public QGLWidget
{
	Q_OBJECT
public:
#if defined(MEEGO_EDITION_HARMATTAN)
	static const int Width = 854.0f;
	static const int Height = 480.0f;
#elif defined(Q_WS_MAEMO_5)
	static const int Width = 800.0f;
	static const int Height = 480.0f;
#endif

	explicit HostVideo(HostInput *hostInput,
					   Emu *emu,
					   EmuThread *thread,
					   QWidget *parent = 0);
	~HostVideo();

	bool isFpsVisible() const;
	void setFpsVisible(bool on);

	bool keepApsectRatio() const;
	void setKeepAspectRatio(bool on);

	bool bilinearFiltering() const;
	void setBilinearFiltering(bool enabled);

	QRectF dstRect() const;

	QPoint convertCoordHostToEmu(const QPoint &hostPos);

	void setShader(const QString &shaderName);
	QString shader() const;
	QStringList shaderList() const;
signals:
	void shaderChanged();
protected:
	void paintEvent(QPaintEvent *);
	void paintGL();
private slots:
	void updateRects();
private:
	void paintEmuFrame();
	void paintFps(QPainter *painter);
	QString shaderDir() const;
	void setupProgramList();
	bool loadShaderProgram();
	bool configureShaderProgram(const char *vsh, const char *fsh);

	HostInput *m_hostInput;
	Emu *m_emu;
	EmuThread *m_thread;

	QRectF m_srcRect;
	QRectF m_dstRect;

	bool m_fpsVisible;
	int m_fpsCount;
	int m_fpsCounter;
	QTime m_fpsCounterTime;

	bool m_keepAspectRatio;
	bool m_bilinearFiltering;

	QGLShaderProgram *m_program;
	int m_u_pvmMatrixLocation;
	int m_u_displaySizeLocation;
	int m_a_vertexLocation;
	int m_a_texCoordLocation;
	int m_s_textureLocation;
	GLfloat m_vertexArray[8];
	GLfloat m_texCoordArray[8];

	int m_programIndex;
	bool m_programDirty;
	QStringList m_programList;
};

inline QRectF HostVideo::dstRect() const
{ return m_dstRect; }
inline bool HostVideo::isFpsVisible() const
{ return m_fpsVisible; }
inline bool HostVideo::keepApsectRatio() const
{ return m_keepAspectRatio; }
inline bool HostVideo::bilinearFiltering() const
{ return m_bilinearFiltering; }

#endif // HOSTVIDEO_H
