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

#ifndef HOSTINPUT_H
#define HOSTINPUT_H

class IMachine;
class TouchInputDevice;
class AccelInputDevice;
class SixAxisInputDevice;
#include <QObject>
class QPainter;

class HostInput : public QObject {
    Q_OBJECT
public:
	explicit HostInput(IMachine *machine);
	~HostInput();

	qreal padOpacity() const;
	void setPadOpacity(qreal opacity);

	QList<QObject *> devices() const;

	void update();

	void paint(QPainter &painter);
signals:
	void pause();
	void quit();
	void devicesChanged();
protected:
	bool eventFilter(QObject *o, QEvent *e);
private slots:
	void onSixAxisDetected();
	void onSixAxisDestroyed();
private:
	void processKey(Qt::Key key, bool state);
	void processTouch(QEvent *e);

	IMachine *m_machine;
	TouchInputDevice *m_touchInputDevice;
	AccelInputDevice *m_accelInputDevice;
	QList<SixAxisInputDevice *> m_sixAxisInputDevices;
	qreal m_padOpacity;
};

inline qreal HostInput::padOpacity() const
{ return m_padOpacity; }

#endif // HOSTINPUT_H
