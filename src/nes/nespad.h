#ifndef NESPAD_H
#define NESPAD_H

#include <QObject>

class NesPad : public QObject {
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
	bool isZapperMode() const;

	void setButtonState(int player, Button button, bool on);
	void clearButtons(int player);
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address);
private:
	void strobe();

	bool m_nextStrobe;
	Buttons m_padA;
	Buttons m_padB;

	quint8 m_padAReg;
	quint8 m_padBReg;
};

inline bool NesPad::isZapperMode() const
{ return false; } // TODO zapper

#endif // NESPAD_H
