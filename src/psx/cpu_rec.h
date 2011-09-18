#ifndef PSXCPUREC_H
#define PSXCPUREC_H

#include "cpu.h"

class PsxCpuRec : public PsxCpu {
public:
	bool init();
	void shutdown();
	void reset();
};

extern PsxCpuRec psxRec;

#endif // PSXCPUREC_H
