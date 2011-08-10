#ifndef RAM8_H
#define RAM8_H

#include "memory8_global.h"
#include <QObject>
#include <QVector>

class MEMORY8_EXPORT Ram8 : public QObject {
	Q_OBJECT
public:
	explicit Ram8(uint size, QObject *parent = 0);
	void write(quint16 address, quint8 data);
	quint8 read(quint16 address) const;
	uint size() const;
signals:
	void sizeChanged();
private:
	QVector<quint8> m_data;
};

inline Ram8::Ram8(uint size, QObject *parent) :
	QObject(parent), m_data(size, Qt::Uninitialized) { }

inline void Ram8::write(quint16 address, quint8 data)
{ m_data.replace(address, data); }
inline quint8 Ram8::read(quint16 address) const
{ return m_data.at(address); }
inline uint Ram8::size() const
{ return m_data.size(); }

#endif // RAM8_H
