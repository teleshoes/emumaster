// port specific settings

#ifndef PORT_CONFIG_H
#define PORT_CONFIG_H

#define CASE_SENSITIVE_FS 1 // CS filesystem
#define DONT_OPEN_MANY_FILES 1
#define REDUCE_IO_CALLS 0

// draw.c
#define OVERRIDE_HIGHCOL 0

// draw2.c
#define START_ROW  0 // which row of tiles to start rendering at?
#define END_ROW   28 // ..end

// logging emu events
#define EL_LOGMASK EL_STATUS // (EL_STATUS|EL_ANOMALY|EL_UIO|EL_SRAMIO|EL_INTS|EL_CDPOLL) // xffff

#define dprintf(x...)

#endif //PORT_CONFIG_H
