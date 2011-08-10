#ifndef ROM8_H
#define ROM8_H

#include "memory8_global.h"
#include <QObject>
#include <QFile>

class MEMORY8_EXPORT Rom8 : public QObject {
	Q_OBJECT
public:
	explicit Rom8(const QString &fileName, QObject *parent = 0);
	~Rom8();
	quint8 read(quint16 address) const;
	uint size() const;
	bool isLoaded() const;
private:
	QFile m_file;
	uchar *m_data;
	uint m_size;
	bool m_mapped;
};

inline quint8 Rom8::read(quint16 address) const
{ return m_data[address]; }
inline uint Rom8::size() const
{ return m_size; }
inline bool Rom8::isLoaded() const
{ return m_data != 0; }

#endif // ROM8_H
