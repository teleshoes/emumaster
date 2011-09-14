#ifndef GTENEON_H
#define GTENEON_H

#if defined(__cplusplus)
extern "C" {
#endif

void gteRTPS_neon(void *cp2_regs, int opcode);
void gteRTPT_neon(void *cp2_regs, int opcode);
void gteMVMVA_neon(void *cp2_regs, int opcode);
void gteNCLIP_neon(void *cp2_regs, int opcode);

#if defined(__cplusplus)
}
#endif

#endif // GTENEON_H
