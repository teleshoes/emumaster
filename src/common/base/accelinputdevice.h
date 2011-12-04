#ifndef ACCELINPUTDEVICE_H
#define ACCELINPUTDEVICE_H

#include "hostinputdevice.h"
#include <QAccelerometer>
#include <QVector3D>

QTM_USE_NAMESPACE

class AccelInputDevice : public HostInputDevice {
    Q_OBJECT
public:
    explicit AccelInputDevice(QObject *parent = 0);
	void update(int *data);

	Q_INVOKABLE void calibrate(const QVector3D &init,
							   const QVector3D &up,
							   const QVector3D &right);
private slots:
	void onConfChanged();
	void onReadingChanged();
private:
	void setEnabled(bool on);
	void convert();

	QAccelerometer *m_accelerometer;
	QVector3D m_upVector;
	QVector3D m_rightVector;

	int m_buttons;
	QVector3D m_read;
	bool m_converted;
};

#endif // ACCELINPUTDEVICE_H