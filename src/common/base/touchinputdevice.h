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
class QPainter;

class TouchInputDevice : public HostInputDevice {
    Q_OBJECT
public:
    explicit TouchInputDevice(QObject *parent = 0);
	void update(int *data);
	void processTouch(QEvent *e);
	void paint(QPainter &painter);
private slots:
	void onConfChanged();
private:
	static const int MaxPoints = 4;
	static const int CircleSize = 240;
	static const int ButtonWidth = 80;
	static const int ButtonHeight = 64;

	void convertPad();
	void convertMouse();
	int buttonsInCircle(int x, int y) const;
	void paintCircle(QPainter &painter, int buttons);

	int m_numPoints;
	QPoint m_points[MaxPoints];

	int m_converted;
	int m_buttons;
	int m_mouseX;
	int m_mouseY;

	QImage m_padImage;
};

#endif // TOUCHINPUTDEVICE_H
