/*
	Free for non-commercial use.
	For commercial use, separate licencing terms must be obtained.
	(c) Copyright 2011, elemental
*/

#ifndef CHEAT_H
#define CHEAT_H

#include <imachine.h>

class PicoCheat {
public:
	PicoCheat();
	bool parse(const QString &code);
	bool active() const;
	void setActive(bool active);
private:
	bool decodeGenie(const QString &code);
	bool decodeHex(const QString &code);
	void write(u16 data);

	u32 m_addr;
	u16 m_data;

	u16 m_oldData;
	bool m_active;
};

#endif // CHEAT_H
