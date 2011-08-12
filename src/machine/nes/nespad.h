#ifndef NESPAD_H
#define NESPAD_H

#include "nes_global.h"
#include <QObject>

class NES_EXPORT NesPad : public QObject {
	Q_OBJECT
public:
	enum Button {
		A = 0x01,
		B = 0x02,
		Select = 0x04,
		Start = 0x08,
		Up = 0x10,
		Down = 0x20,
		Left = 0x40,
		Right = 0x80
	};
	Q_DECLARE_FLAGS(Buttons, Button)

	explicit NesPad(QObject *parent = 0);
	void reset();
	void setButtonState(Button button, bool state);
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);
private:
	Buttons m_buttons;
	int m_pointer;
};

#endif // NESPAD_H
