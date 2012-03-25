#ifndef NESSYNC_H
#define NESSYNC_H

#include <base/emu.h>

static const int NesDmaCycles = 513;

typedef int NesSync(int additionalCycles);

extern bool nesSyncInit(QString *error);
extern void nesSyncShutdown();

extern void nesSyncReset();
extern void nesSyncFrame(bool drawEnabled);

extern u64 nesSyncCpuCycles();

extern void nesSyncSl();

#endif // NESSYNC_H
