#ifndef NESPPU_H
#define NESPPU_H

class NesMachine;
class NesPpuMemoryMapper;
#include "nes_global.h"
#include <video/nes2c0x.h>

class NES_EXPORT NesPpu : public Nes2C0XPpu {
	Q_OBJECT
public:
	explicit NesPpu(NesMachine *machine);
	void setMemory(NesPpuMemoryMapper *memory);
protected:
	void  write(quint16 address, quint8 data);
	quint8 read(quint16 address);
private:
	NesPpuMemoryMapper *m_memory;
};

#endif // NESPPU_H
