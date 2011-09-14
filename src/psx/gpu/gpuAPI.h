#ifndef _GPU_API_
#define _GPU_API_

#include "../psxcommon.h"
#include "../plugins.h"

typedef unsigned long BOOL;

///////////////////////////////////////////////////////////////////////////////
//  GPU global definitions
#define	FRAME_BUFFER_SIZE	(1024*512*2)
#define	FRAME_WIDTH			  1024
#define	FRAME_HEIGHT		  512
#define	FRAME_OFFSET(x,y)	(((y)<<10)+(x))

typedef struct GPUPacket
{
  union
  {
    u32 U4[16];
    s32 S4[16];
    u16 U2[32];
    s16 S2[32];
    u8  U1[64];
	  s8  S1[64];
  };
} GPUPacket;

#ifdef __cplusplus
extern "C" {
#endif

extern u32  GPU_GP1;
extern u16  GPU_FrameBuffer[FRAME_BUFFER_SIZE/2];    // FRAME_BUFFER_SIZE is defined in bytes
extern GPUPacket PacketBuffer;
extern GPUPacket PacketBufferBackup;
extern int  vBlank;

extern void (*GPU_updateLace)(void);
void GPUupdateLace(void);

#ifdef __cplusplus
}
#endif

#endif //_GPU_API_
