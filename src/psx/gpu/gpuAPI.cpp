#include "gpuAPI.h"

u32   GPU_GP1;
u16   GPU_FrameBuffer[FRAME_BUFFER_SIZE/2];    // FRAME_BUFFER_SIZE is defined in bytes
GPUPacket PacketBuffer;
GPUPacket PacketBufferBackup;
void  (*GPU_updateLace)        (void);
