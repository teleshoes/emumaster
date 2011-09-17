#ifndef NESDISK_H
#define NESDISK_H

#include <imachine.h>

class NesDisk {
public:
	bool load(const QString &fileName);

	bool hasBatteryBackedRam() const;
};

extern u32 nesDiskCrc;
extern NesDisk nesDisk;

#endif // NESDISK_H
