#include "newGPU.h"
#include "fixed.h"

void gpuReset(void) {
	GPU_GP1 = 0x14802000;
	TextureWindow[0] = 0;
	TextureWindow[1] = 0;
	TextureWindow[2] = 255;
	TextureWindow[3] = 255;
	DrawingArea[2] = 256;
	DrawingArea[3] = 240;
	DisplayArea[2] = 256;
	DisplayArea[3] = 240;
	DisplayArea[6] = 256;
	DisplayArea[7] = 240;
	Skip = 0;
}

BOOL  gpuInnerInit();

extern "C" long GPU_init(void) {
	GPU_updateLace = GPUupdateLace;
	vBlank = 0;
	gpuInnerInit();
	gpuReset();
	xInit();
	return (0);
}

extern "C" void GPU_vBlank( int val ) {
	if (!val) {
		/*	NOTE: GP1 must have the interlace bit toggle here,
			since it shouldn't be in readStatus as it was previously */
		GPU_GP1 ^= 0x80000000;
	}
	vBlank = val;
}

extern "C" uint32_t GPU_readStatus(void) {
	return GPU_GP1 | (vBlank ? 0x80000000 : 0 );
}
