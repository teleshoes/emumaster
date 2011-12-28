#ifndef PICOMCDPCM_H
#define PICOMCDPCM_H

#include <imachine.h>

#define PCM_STEP_SHIFT 11

extern "C" {
void picoMcdPcmWrite(uint a, u8 d);
void picoMcdPcmSetRate(int rate);
void picoMcdPcmUpdate(int *buffer, int length);
}

#endif // PICOMCDPCM_H
