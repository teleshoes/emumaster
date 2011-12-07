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

#include "accelinputdevice.h"
#include "imachine.h"

// TODO configure possibilty

AccelInputDevice::AccelInputDevice(QObject *parent) :
	HostInputDevice("accel", parent) {
	m_accelerometer = 0;

	m_upVector = QVector3D(0.0f, 0.0f, 9.8f);
	m_rightVector = QVector3D(0.0f, -9.8f, 0.0f);

	QObject::connect(this, SIGNAL(confChanged()), SLOT(onConfChanged()));
}

void AccelInputDevice::onConfChanged() {
	m_converted = false;
	m_buttons = 0;
	setEnabled(confIndex() > 0);
}

void AccelInputDevice::setEnabled(bool on) {
	if (on == (m_accelerometer != 0))
		return;
	if (!on) {
		delete m_accelerometer;
		m_accelerometer = 0;
	} else {
		m_accelerometer = new QAccelerometer(this);
		QObject::connect(m_accelerometer, SIGNAL(readingChanged()),
						 SLOT(onReadingChanged()));
		m_accelerometer->start();
	}
}

void AccelInputDevice::calibrate(const QVector3D &init,
								 const QVector3D &up,
								 const QVector3D &right) {
	m_upVector = up - init;
	m_rightVector = right - init;
}

void AccelInputDevice::update(int *data) {
	if (confIndex() <= 0)
		return;

	if (!m_converted) {
		convert();
		m_converted = true;
	}
	int *pad = IMachine::padOffset(data, confIndex()-1);
	*pad |= m_buttons;
}

void AccelInputDevice::onReadingChanged() {
	QAccelerometerReading *reading = m_accelerometer->reading();
	m_read = QVector3D(reading->x(), reading->y(), reading->z());
	m_converted = false;
}

void AccelInputDevice::convert() {
	qreal up = QVector3D::dotProduct(m_read, m_upVector);
	qreal right = QVector3D::dotProduct(m_read, m_rightVector);

	qreal level = (9.8f*9.8f) * 0.5f;

	m_buttons = 0;
	if (up > level)
		m_buttons |= IMachine::PadKey_Up;
	else if (up < -level)
		m_buttons |= IMachine::PadKey_Down;

	if (right > level)
		m_buttons |= IMachine::PadKey_Left;
	else if (right < -level)
		m_buttons |= IMachine::PadKey_Right;
}
