#ifndef NESPAD_H
#define NESPAD_H

#include <imachine.h>

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

	void init();
	bool isZapperMode() const;

	void setKeys(int pad, int keys);
	void write(u16 address, u8 data);
	u8 read(u16 address);
private:
	void strobe();
};

inline bool NesPad::isZapperMode() const
{ return false; } // TODO zapper

extern NesPad nesPad;

#endif // NESPAD_H
