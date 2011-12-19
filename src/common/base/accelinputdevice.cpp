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
#include <QSettings>

AccelInputDevice::AccelInputDevice(QObject *parent) :
	HostInputDevice("accel", parent) {
	m_accelerometer = 0;

	QObject::connect(this, SIGNAL(confChanged()), SLOT(onConfChanged()));

	QSettings s;
	s.beginGroup("accelerometer");

	m_upVector.setX(s.value("up.x", 0.0f).toReal());
	m_upVector.setY(s.value("up.y", 0.0f).toReal());
	m_upVector.setZ(s.value("up.z", 9.8f).toReal());

	m_rightVector.setX(s.value("right.x", 0.0f).toReal());
	m_rightVector.setY(s.value("right.y", 9.8f).toReal());
	m_rightVector.setZ(s.value("right.z", 0.0f).toReal());

	s.endGroup();
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

	QSettings s;
	s.beginGroup("accelerometer");

	s.setValue("up.x", m_upVector.x());
	s.setValue("up.y", m_upVector.y());
	s.setValue("up.z", m_upVector.z());

	s.setValue("right.x", m_rightVector.x());
	s.setValue("right.y", m_rightVector.y());
	s.setValue("right.z", m_rightVector.z());

	s.endGroup();
}

void AccelInputDevice::update(int *data) {
	if (confIndex() <= 0)
		return;

	if (!m_converted) {
		convert();
		m_converted = true;
	}
	int *pad = IMachine::padOffset(data, confIndex()-1);
	pad[0] |= m_buttons;
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
		m_buttons |= IMachine::PadKey_Right;
	else if (right < -level)
		m_buttons |= IMachine::PadKey_Left;
}
