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
	Q_PROPERTY(int romSize READ romSize CONSTANT)
	Q_PROPERTY(int vromSize READ vromSize CONSTANT)
	Q_PROPERTY(int ramSize READ ramSize CONSTANT)
	Q_PROPERTY(bool hasBatteryBackedRam READ hasBatteryBackedRam CONSTANT)
	Q_PROPERTY(bool hasTrainer READ hasTrainer CONSTANT)
	Q_PROPERTY(bool isVSSystem READ isVSSystem CONSTANT)
	Q_PROPERTY(quint32 crc READ crc CONSTANT)
	Q_PROPERTY(quint8 mapperType READ mapperType CONSTANT)
	Q_PROPERTY(bool isPAL READ isPAL CONSTANT)
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

	bool isPAL() const;

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
inline bool NesDisk::isPAL() const
{ return machineType() == NesMachine::PAL; }

#endif // NESDISK_H
