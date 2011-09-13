#include "newGPU.h"
#include "raster.h"
#include "../machine.h"
#include <QImage>

extern "C" void convert_screen_BGR555_RGB565(void *dst, void *src, int width, int height);

void gpuVideoOutput() {
	int x0 = DisplayArea[0];
	int y0 = DisplayArea[1];

	int w0 = DisplayArea[2];
	int h0 = DisplayArea[3];  // video mode

	int h1 = DisplayArea[7] - DisplayArea[5]; // display needed
	if (h0 == 480)
		h1 = Min2(h1*2,480);

	u32* src  = (u32*)(&((u16*)GPU_FrameBuffer)[FRAME_OFFSET(x0,y0)]);
	int width = (w0+63)/64*64;
	QImage::Format fmt = (GPU_GP1 & 0x00200000 ? QImage::Format_RGB888 : QImage::Format_RGB16);
	if (width != gpuFrame.width() || h1 != gpuFrame.height() || fmt != gpuFrame.format()) {
		if (fmt == QImage::Format_RGB16) {
			gpuFrame = QImage(width, h1, QImage::Format_RGB16);
			qDebug("16 bit video %d %d", w0, h1);
		} else {
			qDebug("24 bit video");
			gpuFrame = QImage((uchar *)src, width, h1, FRAME_WIDTH*2, QImage::Format_RGB888);
		}
		psxMachine.updateGpuScale(w0, h1);
	} else {
		u32* dst = (u32*)gpuFrame.bits();
		if (fmt == QImage::Format_RGB16) {
//			// TODO asm version
//			for (int y = 0; y < h1; y++) {
//				for(int x = 0; x < w0>>1; x++) {
//					const u32 pixbgr = src[x];
//					*dst++ = ((pixbgr&(0x1f001f<<10))>>10) | ((pixbgr&(0x1f001f<<5))<<1) | ((pixbgr&(0x1f001f<<0))<<11);
//				}
//				src += 512;
//			}
			convert_screen_BGR555_RGB565(dst, src, (w0+63)/64*64, h1);
		} else {
			gpuFrame = QImage((uchar *)src, width, h1, FRAME_WIDTH*2, QImage::Format_RGB888);
		}
	}
	PROFILE_RESET(gpuPolyTime,gpuPolyCount);
	PROFILE_RESET(gpuRasterTime,gpuRasterCount);
	PROFILE_RESET(gpuPixelTime,gpuPixelCount);
	PROFILE_RESET(dmaChainTime,dmaChainCount);
	PROFILE_RESET(dmaMemTime,dmaMemCount);
}

extern "C" void GPUupdateLace() {
	if (GPU_GP1 & 0x00800000) // Display disabled
		return;
	if (!Skip)
		gpuVideoOutput();
	psxMachine.flipScreen();
	// TODO ApplyCheats();
}
