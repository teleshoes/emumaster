#include "nesdisk.h"
#include "nesdisk_p.h"
#include <crc32.h>
#include <QAbstractFileEngine>

// TODO some performance hacking
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
	m_data = new quint8[m_file.size()];
	if (m_file.read(reinterpret_cast<char *>(m_data), m_file.size()) != m_file.size()) {
		delete [] m_data;
		m_data = 0;
	}
	if (!m_data)
		return;

	uint offset = 16;
	if (hasTrainer()) {
		m_trainer = m_data + offset;
		offset += 512;
	}
	m_rom = m_data + offset;
	offset += romSize();
	m_vrom = m_data + offset;

	m_mapperType = (m_header->m_flagsA >> 4) | (m_header->m_flagsB & 0xF0);

	computeChecksum();
	patchRom();
}

NesDisk::~NesDisk() {
	delete [] m_data;
}

quint8 *NesDisk::trainer() const
{ return m_trainer; }
quint8 *NesDisk::rom() const
{ return m_rom; }
quint8 *NesDisk::vrom() const
{ return m_vrom; }

NesMapper::Mirroring NesDisk::mirroring() const {
	if (m_header->m_flagsA & NesDiskHeader::FourScreenFlagA)
		return NesMapper::FourScreen;
	else if (m_header->m_flagsA & NesDiskHeader::VerticalFlagA)
		return NesMapper::Vertical;
	else
		return NesMapper::Horizontal;
}
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
{ return m_header->m_flagsA & NesDiskHeader::BatteryBackedRamFlagA; }
bool NesDisk::hasTrainer() const
{ return m_header->m_flagsA & NesDiskHeader::TrainerFlagA; }
bool NesDisk::isVSSystem() const
{ return m_header->m_flagsB & NesDiskHeader::VSSystemFlagB; }

void NesDisk::computeChecksum() {
	if (hasTrainer())
		m_crc = qChecksum32(reinterpret_cast<const char *>(m_trainer), romSize()+512);
	else
		m_crc = qChecksum32(reinterpret_cast<const char *>(m_rom), romSize());
}
