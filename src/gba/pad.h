#ifndef GBAPAD_H
#define GBAPAD_H

#include <QObject>

class GbaPad : public QObject {
	Q_OBJECT
public:
	enum PadKey {
		L_PadKey		= 0x200,
		R_PadKey		= 0x100,
		Down_PadKey		=  0x80,
		Up_PadKey		=  0x40,
		Left_PadKey		=  0x20,
		Right_PadKey	=  0x10,
		Start_PadKey	=  0x08,
		Select_PadKey	=  0x04,
		B_PadKey		=  0x02,
		A_PadKey		=  0x01,
		All_PadKeys		= 0x3FF
	};
	explicit GbaPad(QObject *parent = 0);
	void setKey(PadKey key, bool on);
private:
	int m_keys;
};

#endif // GBAPAD_H
