#ifndef PSXUNAIGPU_H
#define PSXUNAIGPU_H

#include "gpu.h"

class PsxGpuUnai : public PsxGpu {
public:
	bool init();
	void shutdown();
	const QImage &frame();
	void setSkip(bool skip);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

extern PsxGpuUnai psxUnaiGpu;

#endif  // PSXUNAIGPU_H
