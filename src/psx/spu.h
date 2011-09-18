#ifndef PSXSPU_H
#define PSXSPU_H

#include "common.h"
#include "decode_xa.h"

// TODO register callback spuirq

typedef void (* SPUwriteRegister)(u32, u16);
typedef u16 (* SPUreadRegister)(u32);
typedef void (* SPUwriteDMA)(u16);
typedef u16 (* SPUreadDMA)();
typedef void (* SPUwriteDMAMem)(u16 *, int);
typedef void (* SPUreadDMAMem)(u16 *, int);
typedef void (* SPUplayADPCMchannel)(xa_decode_t *);
typedef void (* SPUregisterCallback)(void ( *callback)(void));
typedef void (* SPUasync)(u32);
typedef void (* SPUplayCDDAchannel)(s16 *, int);

#if defined(__cplusplus)

class PsxSpu {
public:
	virtual bool init() = 0;
	virtual void shutdown();

	SPUwriteRegister    writeRegister;
	SPUreadRegister     readRegister;
	SPUwriteDMA         writeDMA;
	SPUreadDMA          readDMA;
	SPUwriteDMAMem      writeDMAMem;
	SPUreadDMAMem       readDMAMem;
	SPUplayADPCMchannel playADPCMchannel;
	SPUregisterCallback registerCallback;
	SPUasync            async;
	SPUplayCDDAchannel  playCDDAchannel;

	virtual bool save(QDataStream &s) = 0;
	virtual bool load(QDataStream &s) = 0;
};

extern PsxSpu *psxSpu;

extern "C" {
#endif

extern SPUwriteRegister    SPU_writeRegister;
extern SPUreadRegister     SPU_readRegister;
extern SPUwriteDMA         SPU_writeDMA;
extern SPUreadDMA          SPU_readDMA;
extern SPUwriteDMAMem      SPU_writeDMAMem;
extern SPUreadDMAMem       SPU_readDMAMem;
extern SPUplayADPCMchannel SPU_playADPCMchannel;
extern SPUregisterCallback SPU_registerCallback;
extern SPUasync            SPU_async;
extern SPUplayCDDAchannel  SPU_playCDDAchannel;

extern void SPUirq();

#if defined(__cplusplus)
}
#endif

#endif // PSXSPU_H
