#include "spu.h"
#include "mem.h"

PsxSpu *psxSpu = 0;

SPUwriteRegister      SPU_writeRegister;
SPUreadRegister       SPU_readRegister;
SPUwriteDMA           SPU_writeDMA;
SPUreadDMA            SPU_readDMA;
SPUwriteDMAMem        SPU_writeDMAMem;
SPUreadDMAMem         SPU_readDMAMem;
SPUplayADPCMchannel   SPU_playADPCMchannel;
SPUregisterCallback   SPU_registerCallback;
SPUasync              SPU_async;
SPUplayCDDAchannel    SPU_playCDDAchannel;

void PsxSpu::shutdown() {
}

void SPUirq() {
	psxHu32ref(0x1070) |= SWAPu32(0x200);
}