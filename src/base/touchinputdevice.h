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

#ifndef TOUCHINPUTDEVICE_H
#define TOUCHINPUTDEVICE_H

#include "hostinputdevice.h"
#include <QPoint>
#include <QImage>
#include <QFeedbackHapticsEffect>
#include <QPainterPath>
#include <QColor>
class QPainter;

QTM_USE_NAMESPACE

class BASE_EXPORT TouchInputDevice : public HostInputDevice
{
	Q_OBJECT
	Q_PROPERTY(bool gridVisible READ isGridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
	Q_PROPERTY(int dpadAreaSize READ dpadAreaSize WRITE setDpadAreaSize NOTIFY dpadAreaSizeChanged)
	Q_PROPERTY(int dpadDiagonalAreaSize READ dpadDiagonalAreaSize WRITE setDpadDiagonalAreaSize NOTIFY dpadDiagonalAreaSizeChanged)
	Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor NOTIFY gridColorChanged)
	Q_PROPERTY(bool buttonsVisible READ areButtonsVisible WRITE setButtonsVisible NOTIFY buttonsVisibleChanged)
public:
	explicit TouchInputDevice(QObject *parent = 0);
	void sync(EmuInput *emuInput);
	void processTouch(QEvent *e);
	void paint(QPainter *painter);
	void setHapticFeedbackEnabled(bool on);
	void setLRVisible(bool on);

	void setGridVisible(bool on);
	bool isGridVisible() const;

	void setDpadAreaSize(int size);
	int dpadAreaSize() const;

	void setDpadDiagonalAreaSize(int size);
	int dpadDiagonalAreaSize() const;

	void setGridColor(const QColor &color);
	QColor gridColor() const;

	void setButtonsVisible(bool on);
	bool areButtonsVisible() const;

	void setPsxButtonsEnabled(bool on);
	void setPicoButtonsEnabled(bool on);
	void setGbaButtonsEnabled(bool on);
signals:
	void gridVisibleChanged();
	void dpadAreaSizeChanged();
	void dpadDiagonalAreaSizeChanged();
	void gridColorChanged();
	void buttonsVisibleChanged();
private slots:
	void onEmuFunctionChanged();
private:
	static const int MaxPoints = 4;
	static const int ButtonWidth = 80;
	static const int ButtonHeight = 64;
	static const int PaintedButtonSize = 64;

	void setupEmuFunctionList();
	void convertPad();
	void convertMouse();
	int buttonsInDpad(int x, int y) const;
	void addDpadAreaToGrid(int x, int y);

	void updatePaintedButtons();
	void addPaintedButtonsPad();
	void addPaintedButtonsMouse();
	void addPaintedButton(int button, int flag, QPointF pos);
	void updateGrid();
	void addGridPad();

	int m_numPoints;
	QPoint m_points[MaxPoints];

	int m_converted;
	int m_buttons;
	int m_mouseX;
	int m_mouseY;
	int m_lastMouseX;
	int m_lastMouseY;
	bool m_mouseMoving;

	int m_areaSize;
	int m_diagonalAreaSize;
	bool m_gridVisible;
	int m_lrYPos;
	bool m_lrVisible;
	QPainterPath m_grid;
	QColor m_gridColor;

	class PaintedButton {
	public:
		int flag;
		QPointF dst;
		QRectF src;
		QRectF srcPressed;
	};

	bool m_buttonsVisible;
	bool m_psxButtonsEnable;
	bool m_picoButtonsEnable;
	bool m_gbaButtonsEnable;
	QVector<PaintedButton> m_paintedButtons;
	QImage m_buttonsImage;

	QFeedbackHapticsEffect *m_hapticEffect;
};

inline bool TouchInputDevice::isGridVisible() const
{ return m_gridVisible; }

inline int TouchInputDevice::dpadAreaSize() const
{ return m_areaSize; }

inline int TouchInputDevice::dpadDiagonalAreaSize() const
{ return m_diagonalAreaSize; }

inline QColor TouchInputDevice::gridColor() const
{ return m_gridColor; }

inline bool TouchInputDevice::areButtonsVisible() const
{ return m_buttonsVisible; }

#endif // TOUCHINPUTDEVICE_H
