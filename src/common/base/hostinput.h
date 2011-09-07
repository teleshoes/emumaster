#ifndef HOSTINPUT_H
#define HOSTINPUT_H

class IMachine;
#include <QObject>

class HostInput : public QObject {
    Q_OBJECT
public:
	explicit HostInput(IMachine *machine);
	~HostInput();

	void setQuickQuitEnabled(bool on);
signals:
	void pauseClicked();
	void wantClose();
protected:
	bool eventFilter(QObject *o, QEvent *e);
private:
	void processKey(Qt::Key key, bool state);
	void processTouch(QEvent *e);

	IMachine *m_machine;
	bool m_quickQuitEnabled;
};

#endif // HOSTINPUT_H
