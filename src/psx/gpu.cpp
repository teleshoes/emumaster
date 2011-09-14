#include "gpu.h"
#include "gpu/newGPU.h"
#include <QDataStream>

PsxGpu psxGpu;

#define STATE_SERIALIZE_BUILDER(sl) \
STATE_SERIALIZE_BEGIN_##sl(PsxGpu, 1) \
	STATE_SERIALIZE_VAR_##sl(GPU_GP1) \
	STATE_SERIALIZE_VAR_##sl(isPAL) \
	STATE_SERIALIZE_VAR_##sl(isDisplaySet) \
	STATE_SERIALIZE_VAR_##sl(MaskU) \
	STATE_SERIALIZE_VAR_##sl(MaskV) \
	STATE_SERIALIZE_VAR_##sl(Masking) \
	STATE_SERIALIZE_VAR_##sl(PixelMSB) \
	STATE_SERIALIZE_VAR_##sl(vBlank) \
	STATE_SERIALIZE_ARRAY_##sl(GPU_FrameBuffer, sizeof(GPU_FrameBuffer)) \
	STATE_SERIALIZE_ARRAY_##sl(DisplayArea, sizeof(DisplayArea)) \
	STATE_SERIALIZE_ARRAY_##sl(DirtyArea, sizeof(DirtyArea)) \
	STATE_SERIALIZE_ARRAY_##sl(LastDirtyArea, sizeof(LastDirtyArea)) \
	STATE_SERIALIZE_ARRAY_##sl(CheckArea, sizeof(CheckArea)) \
	STATE_SERIALIZE_ARRAY_##sl(TextureWindow, sizeof(TextureWindow)) \
	STATE_SERIALIZE_ARRAY_##sl(DrawingArea, sizeof(DrawingArea)) \
	STATE_SERIALIZE_ARRAY_##sl(DrawingOffset, sizeof(DrawingOffset)) \
STATE_SERIALIZE_END_##sl(PsxGpu)

STATE_SERIALIZE_BUILDER(SAVE)
STATE_SERIALIZE_BUILDER(LOAD)
