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
class JoystickDevice;
class SixAxis;
#include <QAccelerometer>
#include <QObject>

QTM_USE_NAMESPACE

class HostInput : public QObject {
    Q_OBJECT
public:
	explicit HostInput(IMachine *machine);
	~HostInput();

	bool isAccelerometerEnabled() const;
	void setAccelerometerEnabled(bool on);
signals:
	void pauseClicked();
	void wantClose();
protected:
	bool eventFilter(QObject *o, QEvent *e);
private slots:
	void accelerometerUpdated();
	void sixAxisUpdated();
	void sixAxisDetected();
private:
	void processKey(Qt::Key key, bool state);
	void processTouch(QEvent *e);
	void setKeyState(int key, bool state);

	IMachine *m_machine;
	int m_keysPhone;
	QAccelerometer *m_accelerometer;
	SixAxis *m_sixAxis;
	JoystickDevice *m_joy;
};

#endif // HOSTINPUT_H
