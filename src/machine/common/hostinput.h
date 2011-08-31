#ifndef HOSTINPUT_H
#define HOSTINPUT_H

class MachineView;
class IMachine;
#include <QObject>

class HostInput : public QObject {
    Q_OBJECT
	Q_PROPERTY(bool swipeEnable READ isSwipeEnabled WRITE setSwipeEnabled NOTIFY swipeEnableChanged)
	Q_PROPERTY(bool padVisible READ isPadVisible WRITE setPadVisible NOTIFY padVisibleChanged)
public:
	explicit HostInput(MachineView *parent);
	~HostInput();

	bool isSwipeEnabled() const;
	void setSwipeEnabled(bool on);

	bool isPadVisible() const;
	void setPadVisible(bool on);
signals:
	void swipeEnableChanged();
	void padVisibleChanged();
protected:
	bool eventFilter(QObject *o, QEvent *e);
private:
	void processKey(Qt::Key key, bool state);
	void processTouch(QEvent *e);

	bool m_swipeEnabled;
	bool m_padVisible;
	IMachine *m_machine;
};

inline bool HostInput::isSwipeEnabled() const
{ return m_swipeEnabled; }
inline bool HostInput::isPadVisible() const
{ return m_padVisible; }

#endif // HOSTINPUT_H
