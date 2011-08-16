#ifndef NESDISK_H
#define NESDISK_H

#include "nes_global.h"
#include "nesmachine.h"
#include "nesppumapper.h"
#include <QObject>
#include <QFile>

class NesDiskHeader;

class NES_EXPORT NesDisk : public QObject {
	Q_OBJECT
public:
	explicit NesDisk(const QString &fileName, QObject *parent = 0);
	~NesDisk();
	bool isLoaded() const;

	quint8 *trainer() const;
	quint8 *rom() const;
	quint8 *vrom() const;

	quint8 mapperType() const;
	int romSize() const;
	int vromSize() const;
	int ramSize() const;

	NesMachine::Type machineType() const;
	NesPpuMapper::Mirroring mirroring() const;
	bool hasBatteryBackedRam() const;
	bool hasTrainer() const;
	bool isVSSystem() const;

	quint32 crc() const;
private:
	void computeChecksum();
	void patchRom();

	QFile m_file;
	union {
		NesDiskHeader *m_header;
		quint8 *m_data;
	};
	quint8 *m_trainer;
	quint8 *m_rom;
	quint8 *m_vrom;
	quint8 m_mapperType;

	quint32 m_crc;
};

inline quint32 NesDisk::crc() const
{ return m_crc; }
inline quint8 NesDisk::mapperType() const
{ return m_mapperType; }

#endif // NESDISK_H
