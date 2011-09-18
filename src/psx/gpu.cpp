#include "gpu.h"

PsxGpu *psxGpu = 0;

GPUwriteStatus        GPU_writeStatus;
GPUwriteData          GPU_writeData;
GPUwriteDataMem       GPU_writeDataMem;
GPUreadStatus         GPU_readStatus;
GPUreadData           GPU_readData;
GPUreadDataMem        GPU_readDataMem;
GPUdmaChain           GPU_dmaChain;
GPUupdateLace         GPU_updateLace;

void PsxGpu::shutdown() {
}
