#ifndef NESMAPPEREEPROM_H
#define NESMAPPEREEPROM_H

#include <QObject>
// TODO make as component

class X24C01 : public QObject {
	Q_OBJECT
public:
	explicit X24C01(QObject *parent = 0);
	~X24C01();

	void reset(quint8 *eedata);
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
	quint8 m_address;
	quint8 m_data;
	bool m_sda;
	bool m_sclOld;
	bool m_sdaOld;

	quint8 *m_eedata;
};

class X24C02 : public QObject {
	Q_OBJECT
public:
	explicit X24C02(QObject *parent = 0);
	~X24C02();

	void reset(quint8 *eedata);
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
	quint8 m_address;
	quint8 m_data;
	quint8 m_rw;
	bool m_sda;
	bool m_sclOld, m_sdaOld;

	quint8 *m_eedata;
};

#endif // NESMAPPEREEPROM_H
