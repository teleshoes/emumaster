#include "rom8.h"
#include <QAbstractFileEngine>

Rom8::Rom8(const QString &fileName, QObject *parent) :
	QObject(parent),
	m_file(fileName),
	m_data(0),
	m_size(0),
	m_mapped(false) {
	if (!m_file.open(QIODevice::ReadOnly))
		return;
	m_size = m_file.size();
	m_size = qMin(m_size, uint(0x10000));
	if (m_file.fileEngine()->supportsExtension(QAbstractFileEngine::MapExtension)
			&& m_file.fileEngine()->QAbstractFileEngine::supportsExtension(QAbstractFileEngine::UnMapExtension)) {
		m_data = m_file.map(0, m_size);
		m_mapped = true;
	} else {
		m_data = new uchar[m_size];
		if (m_file.read(reinterpret_cast<char *>(m_data), m_size) != m_size) {
			delete [] m_data;
			m_data = 0;
		}
	}
}

Rom8::~Rom8() {
	if (!m_mapped)
		delete [] m_data;
}
