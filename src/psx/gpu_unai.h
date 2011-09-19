#ifndef PSXUNAIGPU_H
#define PSXUNAIGPU_H

#include "gpu.h"

class PsxGpuUnai : public PsxGpu {
public:
	bool init();
	void shutdown();
	const QImage &frame();
	void setDrawEnabled(bool drawEnabled);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

extern PsxGpuUnai psxGpuUnai;

#endif  // PSXUNAIGPU_H
