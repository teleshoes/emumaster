#ifndef NESDISK_H
#define NESDISK_H

#include "nes_global.h"
#include "nesmachine.h"
#include "nesppumemorymapper.h"
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
	NesPpuMemoryMapper::Mirroring mirroring() const;
	bool hasBatteryBackedRam() const;
	bool hasTrainer() const;
	bool isVSSystem() const;
private:
	QFile m_file;
	union {
		NesDiskHeader *m_header;
		quint8 *m_data;
	};
	bool m_mapped;
	quint8 *m_trainer;
	quint8 *m_rom;
	quint8 *m_vrom;
};

#endif // NESDISK_H
