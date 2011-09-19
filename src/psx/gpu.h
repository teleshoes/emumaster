#ifndef PSXGPU_H
#define PSXGPU_H

#include "common.h"

typedef void (* GPUwriteStatus)(u32);
typedef void (* GPUwriteData)(u32);
typedef void (* GPUwriteDataMem)(u32 *, int);
typedef u32 (* GPUreadStatus)();
typedef u32 (* GPUreadData)();
typedef void (* GPUreadDataMem)(u32 *, int);
typedef u32 (* GPUdmaChain)(u32 *,u32);
typedef void (* GPUupdateLace)();

#if defined(__cplusplus)

class PsxGpu {
public:
	virtual bool init() = 0;
	virtual void shutdown();
	virtual const QImage &frame() = 0;
	virtual void setDrawEnabled(bool drawEnabled) = 0;

	virtual bool save(QDataStream &s) = 0;
	virtual bool load(QDataStream &s) = 0;
};

extern PsxGpu *psxGpu;

extern "C" {
#endif

extern GPUreadStatus    GPU_readStatus;
extern GPUreadData      GPU_readData;
extern GPUreadDataMem   GPU_readDataMem;
extern GPUwriteStatus   GPU_writeStatus;
extern GPUwriteData     GPU_writeData;
extern GPUwriteDataMem  GPU_writeDataMem;
extern GPUdmaChain      GPU_dmaChain;
extern GPUupdateLace    GPU_updateLace;

#if defined(__cplusplus)
}
#endif

#endif // PSXGPU_H
