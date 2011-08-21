#ifndef HOSTINPUT_H
#define HOSTINPUT_H

class MachineView;
class IMachine;
#include <QObject>

class HostInput : public QObject {
    Q_OBJECT
	Q_PROPERTY(bool swipeEnable READ isSwipeEnabled WRITE setSwipeEnabled NOTIFY swipeEnableChanged)
public:
	explicit HostInput(MachineView *parent);
	~HostInput();

	bool isSwipeEnabled() const;
	void setSwipeEnabled(bool on);
signals:
	void swipeEnableChanged();
protected:
	bool eventFilter(QObject *o, QEvent *e);
private:
	bool m_swipeEnabled;
	IMachine *m_machine;
};

inline bool HostInput::isSwipeEnabled() const
{ return m_swipeEnabled; }

#endif // HOSTINPUT_H
