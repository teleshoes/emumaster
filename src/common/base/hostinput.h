#ifndef HOSTINPUT_H
#define HOSTINPUT_H

class IMachine;
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
private:
	void processKey(Qt::Key key, bool state);
	void processTouch(QEvent *e);
	void setKeyState(int key, bool state);

	IMachine *m_machine;
	int m_keysPhone;
	QAccelerometer *m_accelerometer;
};

#endif // HOSTINPUT_H
