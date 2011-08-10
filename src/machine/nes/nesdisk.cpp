#include "nesdisk.h"
#include <QAbstractFileEngine>

class NesDiskHeader {
public:
	char m_magic[4];
	quint8 m_num16KBRomBanks;
	quint8 m_num8KBVRomBanks;
	quint8 m_flagsA;
	quint8 m_flagsB;
	quint8 m_num8KBRamBanks;
	quint8 m_flagsC;
} Q_PACKED;

// TODO support for other formats

NesDisk::NesDisk(const QString &fileName, QObject *parent) :
	QObject(parent),
	m_file(fileName),
	m_data(0),
	m_trainer(0),
	m_rom(0),
	m_vrom(0) {

	if (!m_file.open(QIODevice::ReadOnly))
		return;
	m_mapped = m_file.fileEngine()->supportsExtension(QAbstractFileEngine::MapExtension);
	m_mapped &= m_file.fileEngine()->supportsExtension(QAbstractFileEngine::UnMapExtension);
	if (m_mapped)
		m_data = static_cast<quint8 *>(m_file.map(0, m_file.size()));
	else {
		m_data = new quint8[m_file.size()];
		if (m_file.read(reinterpret_cast<char *>(m_data), m_file.size()) != m_file.size()) {
			delete [] m_data;
			m_data = 0;
		}
	}
	if (m_data) {
		uint offset = 16;
		if (hasTrainer()) {
			m_trainer = m_data + offset;
			offset += 512;
		}
		m_rom = m_data + offset;
		offset += romSize();
		m_vrom = m_data + offset;
	}
}

NesDisk::~NesDisk() {
	if (!m_mapped)
		delete [] m_data;
}

quint8 *NesDisk::trainer() const
{ return m_trainer; }
quint8 *NesDisk::rom() const
{ return m_rom; }
quint8 *NesDisk::vrom() const
{ return m_vrom; }

NesPpuMemoryMapper::Mirroring NesDisk::mirroring() const {
	if (m_header->m_flagsA & 8)
		return NesPpuMemoryMapper::FourScreen;
	else if (m_header->m_flagsA & 1)
		return NesPpuMemoryMapper::Vertical;
	else
		return NesPpuMemoryMapper::Horizontal;
}
quint8 NesDisk::mapperType() const
{ return (m_header->m_flagsA >> 4) | (m_header->m_flagsB & 0xF0); }
int NesDisk::romSize() const
{ return m_header->m_num16KBRomBanks * 0x4000; }
int NesDisk::vromSize() const
{ return m_header->m_num8KBVRomBanks * 0x2000; }
int NesDisk::ramSize() const
{ return m_header->m_num8KBRamBanks * 0x2000; }

bool NesDisk::isLoaded() const {
	if (!m_data)
		return false;
	return qstrncmp(m_header->m_magic, "NES\x1A", 4) == 0;
}

NesMachine::Type NesDisk::machineType() const
{ return (m_header->m_flagsC & 0x01) ? NesMachine::PAL : NesMachine::NTSC; }
bool NesDisk::hasBatteryBackedRam() const
{ return m_header->m_flagsA & 0x02; }
bool NesDisk::hasTrainer() const
{ return m_header->m_flagsA & 0x04; }
bool NesDisk::isVSSystem() const
{ return m_header->m_flagsB & 0x01; }
