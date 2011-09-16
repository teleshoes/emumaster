#ifndef EEPROM_H
#define EEPROM_H

#include <imachine.h>

class X24C01 {
public:
	void reset(u8 *eedata);
	void write(bool scl_in, bool sda_in);
	bool read() const;

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	enum State {
		Idle,
		Address,
		Read,
		Write,
		Ack,
		AckWait
	};
	int m_nowState;
	int m_nextState;
	int m_bitCounter;
	u8 m_address;
	u8 m_data;
	bool m_sda;
	bool m_sclOld;
	bool m_sdaOld;

	u8 *m_eedata;
};

class X24C02 {
public:
	void reset(u8 *eedata);
	void write(bool scl_in, bool sda_in);
	bool read() const;

	bool save(QDataStream &s);
	bool load(QDataStream &s);
private:
	enum State {
		Idle,
		DeviceAddress,
		Address,
		Read,
		Write,
		Ack,
		Nak,
		AckWait
	};
	int m_nowState;
	int m_nextState;
	int m_bitCounter;
	u8 m_address;
	u8 m_data;
	u8 m_rw;
	bool m_sda;
	bool m_sclOld, m_sdaOld;

	u8 *m_eedata;
};

#endif // EEPROM_H
