#ifndef NESDISK_H
#define NESDISK_H

#include <imachine.h>

class NesDisk {
public:
	int load(const QString &fileName);

	bool hasBatteryBackedRam() const;
};

extern u32 nesDiskCrc;
extern NesDisk nesDisk;

#endif // NESDISK_H
