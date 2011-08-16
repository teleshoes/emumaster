#include "nesppumapper.h"
#include "nesmapper.h"
#include "nesmachine.h"
#include "nesdisk.h"
#include "nesppu.h"
#include <QDataStream>

NesPpuMapper::NesPpuMapper(NesMapper *mapper) :
	QObject(mapper) {
	qMemSet(m_banks, 0, sizeof(m_banks));
	for (int i = 0; i < 16; i++)
		m_banksType[i] = VramBank;
	qMemSet(m_vram, 0, sizeof(m_vram));
	qMemSet(m_cram, 0, sizeof(m_cram));

	NesMachine *machine = mapper->machine();
	NesDisk *disk = machine->disk();
	m_vrom = disk->vrom();
	Q_ASSERT(m_vrom != 0);
	m_vromSize = disk->vromSize();
	m_defaultMirroring = disk->mirroring();

	NesPpu *ppu = machine->ppu();
	ppu->setRenderMethod(NesPpu::PreRender);
	ppu->setCharacterLatchEnabled(false);
	ppu->setExternalLatchEnabled(false);

	setMirroring(m_defaultMirroring);
	setVrom8KBank(0);
}

NesPpuMapper::~NesPpuMapper() {
}

NesMapper *NesPpuMapper::mapper() const
{ return static_cast<NesMapper *>(parent()); }

void NesPpuMapper::reset() {
}

void NesPpuMapper::setMirroring(Mirroring mirroring) {
	if (mirroring == Vertical)
		setMirroring(0, 1, 0, 1);
	else if (mirroring == Horizontal)
		setMirroring(0, 0, 1, 1);
	else if (mirroring == SingleHigh)
		setMirroring(1, 1, 1, 1);
	else if (mirroring == SingleLow)
		setMirroring(0, 0, 0, 0);
	else if (mirroring == FourScreen)
		setMirroring(0, 1, 2, 3);
}

void NesPpuMapper::setMirroring(uint bank0, uint bank1, uint bank2, uint bank3) {
	m_banks[8] = m_banks[12] = m_vram + bank0*0x400;
	m_banks[9] = m_banks[13] = m_vram + bank1*0x400;
	m_banks[10] = m_banks[14] = m_vram + bank2*0x400;
	m_banks[11] = m_banks[15] = m_vram + bank3*0x400;
}

bool NesPpuMapper::save(QDataStream &s) {
	if (s.writeRawData(reinterpret_cast<const char *>(m_vram), sizeof(m_vram)) != sizeof(m_vram))
		return false;
	if (s.writeRawData(reinterpret_cast<const char *>(m_cram), sizeof(m_cram)) != sizeof(m_cram))
		return false;
	for (int i = 0; i < 16; i++) {
		s << quint8(m_banksType[i]);
		if (m_banksType[i] == VromBank)
			s << quint16(m_banks[i] - m_vrom);
		else if (m_banksType[i] == CramBank)
			s << quint16(m_banks[i] - m_cram);
		else if (m_banksType[i] == VramBank)
			s << quint16(m_banks[i] - m_vram);
	}
	return true;
}

bool NesPpuMapper::load(QDataStream &s) {
	if (s.readRawData(reinterpret_cast<char *>(m_vram), sizeof(m_vram)) != sizeof(m_vram))
		return false;
	if (s.readRawData(reinterpret_cast<char *>(m_cram), sizeof(m_cram)) != sizeof(m_cram))
		return false;
	for (int i = 0; i < 16; i++) {
		quint8 bType;
		s >> bType;
		m_banksType[i] = static_cast<BankType>(bType);

		quint16 offset;
		s >> offset;
		if (m_banksType[i] == VromBank)
			m_banks[i] = m_vrom + offset;
		else if (m_banksType[i] == CramBank)
			m_banks[i] = m_cram + offset;
		else if (m_banksType[i] == VramBank)
			m_banks[i] = m_vram + offset;
		else
			return false;
	}
	return true;
}

void NesPpuMapper::horizontalSync(int scanline)
{ Q_UNUSED(scanline) }

void NesPpuMapper::verticalSync()
{ }

void NesPpuMapper::addressBusLatch(quint16 address)
{ Q_UNUSED(address) }

void NesPpuMapper::characterLatch(quint16 address)
{ Q_UNUSED(address) }

void NesPpuMapper::extensionLatchX(uint x)
{ Q_UNUSED(x) }

void NesPpuMapper::extensionLatch(quint16 address, quint8 *plane1, quint8 *plane2, quint8 *attribute) {
	Q_UNUSED(address)
	Q_UNUSED(plane1)
	Q_UNUSED(plane2)
	Q_UNUSED(attribute)
}
