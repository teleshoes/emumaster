#include "nesmappereeprom.h"
#include <QDataStream>

X24C01::X24C01(QObject *parent) :
	QObject(parent),
	m_eedata(0) {
}

X24C01::~X24C01() {
}

void X24C01::reset(quint8 *eedata) {
	m_nowState = Idle;
	m_nextState = Idle;
	m_address = 0;
	m_data = 0;
	m_sda = true;
	m_sclOld = false;
	m_sdaOld = false;

	m_eedata = eedata;
}

void X24C01::write(bool scl_in, bool sda_in) {
	// clock line
	bool scl_rise = (!m_sclOld && scl_in);
	bool scl_fall = (m_sclOld && !scl_in);
	// data line
	bool sda_rise = (!m_sdaOld && sda_in);
	bool sda_fall = (m_sdaOld && !sda_in);

	if (m_sclOld && sda_fall) {
		// start condition
		m_nowState = Address;
		m_bitCounter = 0;
		m_address = 0;
		m_sda = true;
	} else if (m_sclOld && sda_rise) {
		// stop condition
		m_nowState = Idle;
		m_sda = true;
	} else if (scl_rise) {
		switch (m_nowState) {
		case Address:
			if (m_bitCounter < 7) {
				m_address &= ~(1<<m_bitCounter);
				m_address |= (sda_in?1:0)<<m_bitCounter;
			} else {
				if (sda_in) {
					m_nextState = Read;
					m_data = m_eedata[m_address&0x7F];
				} else {
					m_nextState = Write;
				}
			}
			m_bitCounter++;
			break;
		case Ack:
			m_sda = false;
			break;
		case Read:
			if (m_bitCounter < 8)
				m_sda = (m_data&(1<<m_bitCounter)) ? true : false;
			m_bitCounter++;
			break;
		case Write:
			if (m_bitCounter < 8) {
				m_data &= ~(1<<m_bitCounter);
				m_data |= (sda_in?1:0)<<m_bitCounter;
			}
			m_bitCounter++;
			break;

		case AckWait:
			if (!sda_in) {
				m_nextState = Idle;
			}
			break;
		}
	} else if (scl_fall) {
		switch (m_nowState) {
		case Address:
			if (m_bitCounter >= 8) {
				m_nowState = Ack;
				m_sda = true;
			}
			break;
		case Ack:
			m_nowState = m_nextState;
			m_bitCounter = 0;
			m_sda = true;
			break;
		case Read:
			if (m_bitCounter >= 8) {
				m_nowState = AckWait;
				m_address = (m_address+1)&0x7F;
			}
			break;
		case Write:
			if (m_bitCounter >= 8) {
				m_nowState = Ack;
				m_nextState = Idle;
				m_eedata[m_address&0x7F] = m_data;
				m_address = (m_address+1)&0x7F;
			}
			break;
		default:
			break;
		}
	}
	m_sclOld = scl_in;
	m_sdaOld = sda_in;
}

bool X24C01::read() const
{ return m_sda; }

bool X24C01::save(QDataStream &s) {
	s << m_nowState;
	s << m_nextState;
	s << m_bitCounter;
	s << m_address;
	s << m_data;
	s << m_sda;
	s << m_sclOld;
	s << m_sdaOld;
	return true;
}

bool X24C01::load(QDataStream &s) {
	s >> m_nowState;
	s >> m_nextState;
	s >> m_bitCounter;
	s >> m_address;
	s >> m_data;
	s >> m_sda;
	s >> m_sclOld;
	s >> m_sdaOld;
	return true;
}

X24C02::X24C02(QObject *parent) :
	QObject(parent),
	m_eedata(0) {
}

X24C02::~X24C02() {
}

void X24C02::reset(quint8 *eedata) {
	m_nowState = Idle;
	m_nextState = Idle;
	m_address = 0;
	m_data = 0;
	m_rw = 0;
	m_sda = true;
	m_sclOld = false;
	m_sdaOld = false;

	m_eedata = eedata;
}

void X24C02::write(bool scl_in, bool sda_in) {
	// clock line
	bool scl_rise = (!m_sclOld && scl_in);
	bool scl_fall = (m_sclOld && !scl_in);
	// data line
	bool sda_rise = (!m_sdaOld && sda_in);
	bool sda_fall = (m_sdaOld && !sda_in);

	if (m_sclOld && sda_fall) {
		// start condition
		m_nowState = DeviceAddress;
		m_bitCounter = 0;
		m_sda = true;
	} else if (m_sclOld && sda_rise) {
		// stop condition
		m_nowState = Idle;
		m_sda = true;
	} else if (scl_rise) {
		switch (m_nowState) {
		case DeviceAddress:
			if (m_bitCounter < 8) {
				m_data &= ~(1<<(7-m_bitCounter));
				m_data |= (sda_in?1:0)<<(7-m_bitCounter);
			}
			break;
		case Address:
			if (m_bitCounter < 8) {
				m_address &= ~(1<<(7-m_bitCounter));
				m_address |= (sda_in?1:0)<<(7-m_bitCounter);
			}
			m_bitCounter++;
			break;
		case Ack:
			m_sda = false;
			break;
		case Nak:
			m_sda = true;
			break;
		case Read:
			if (m_bitCounter < 8)
				m_sda = (m_data&(1<<(7-m_bitCounter))) ? true : false;
			m_bitCounter++;
			break;
		case Write:
			if (m_bitCounter < 8) {
				m_data &= ~(1<<(7-m_bitCounter));
				m_data |= (sda_in?1:0)<<(7-m_bitCounter);
			}
			m_bitCounter++;
			break;

		case AckWait:
			if (!sda_in) {
				m_nextState = Read;
				m_data = m_eedata[m_address];
			}
			break;
		}
	} else if (scl_fall) {
		switch (m_nowState) {
		case DeviceAddress:
			if (m_bitCounter >= 8) {
				if ((m_data & 0xA0) == 0xA0) {
					m_nowState = Ack;
					m_rw = m_data & 0x01;
					m_sda = true;
					if (m_rw) {
						m_nextState = Read;
						m_data = m_eedata[m_address];
					} else {
						m_nextState = Address;
					}
					m_bitCounter = 0;
				} else {
					m_nowState = Nak;
					m_nextState = Idle;
					m_sda = true;
				}
			}
			break;
		case Address:
			if (m_bitCounter >= 8) {
				m_nowState = Ack;
				m_sda = true;
				if (m_rw) {
					m_nextState = Idle;
				} else {
					m_nextState = Write;
				}
				m_bitCounter = 0;
			}
			break;
		case Read:
			if (m_bitCounter >= 8) {
				m_nowState = AckWait;
				m_address = (m_address+1)&0xFF;
			}
			break;
		case Write:
			if (m_bitCounter >= 8) {
				m_eedata[m_address] = m_data;
				m_nowState = Ack;
				m_nextState = Write;
				m_address = (m_address+1)&0xFF;
				m_bitCounter = 0;
			}
			break;
		case Nak:
			m_nowState = Idle;
			m_bitCounter = 0;
			m_sda = true;
			break;
		case Ack:
			m_nowState = m_nextState;
			m_bitCounter = 0;
			m_sda = true;
			break;
		case AckWait:
			m_nowState = m_nextState;
			m_bitCounter = 0;
			m_sda = true;
			break;
		}
	}
	m_sclOld = scl_in;
	m_sdaOld = sda_in;
}

bool X24C02::read() const
{ return m_sda; }

bool X24C02::save(QDataStream &s) {
	s << m_nowState;
	s << m_nextState;
	s << m_bitCounter;
	s << m_address;
	s << m_data;
	s << m_rw;
	s << m_sda;
	s << m_sclOld;
	s << m_sdaOld;
	return true;
}

bool X24C02::load(QDataStream &s) {
	s >> m_nowState;
	s >> m_nextState;
	s >> m_bitCounter;
	s >> m_address;
	s >> m_data;
	s >> m_rw;
	s >> m_sda;
	s >> m_sclOld;
	s >> m_sdaOld;
	return true;
}
