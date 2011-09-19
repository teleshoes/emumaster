#ifndef PSXGPUUNAI_H
#define PSXGPUUNAI_H

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

#endif  // PSXGPUUNAI_H
