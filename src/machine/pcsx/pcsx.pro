include(../machine.pri)
TEMPLATE = app

DEFINES += GP2X DYNAREC PSXREC NOSOUND

INCLUDEPATH += ix86_64

OTHER_FILES += \
    Makefile.in \
    Makefile.am \
    Makefile \
    gte_neon.S \
    android/video_blit.S \
    gpuAPI/newGPU/ARM_asm.S \
    new_dynarec/linkage_arm.S \
    sound/spucfg-0.1df/dfsound.glade2 \
    sound/Makefile.in \
    sound/Makefile.am \
    video/gpucfg-0.1df/dfxvideo.glade2 \
    video/i386.asm \
    video/Makefile.in \
    video/Makefile.am \
	android/minimal.h \
	android/emumedia.h \
	android/emumedia.cpp \
	android/emulator.cpp \
	android/minimal.c \
	video/macros.inc

HEADERS += \
    plugins.h \
    misc.h \
    minimal.h \
    mdec.h \
    cdriso.h \
    r3000a.h \
    psxmem.h \
    psxhw.h \
    system.h \
    spu.h \
    socket.h \
    sjisfont.h \
    sio.h \
    psxhle.h \
    psxdma.h \
    psxcounters.h \
    psxcommon.h \
    psxbios.h \
    psemu_plugin_defs.h \
    ppf.h \
    config.h \
    coff.h \
    gte_neon.h \
    cheat.h \
    cdrom.h \
    gte_divider.h \
    gte.h \
    decode_xa.h \
    debug.h \
    gpuAPI/gpuAPI.h \
    gpuAPI/newGPU/raster.h \
    gpuAPI/newGPU/profiller.h \
    gpuAPI/newGPU/op_Texture.h \
    gpuAPI/newGPU/op_Light.h \
    gpuAPI/newGPU/op_Blend.h \
    gpuAPI/newGPU/newGPU.h \
    gpuAPI/newGPU/inner_Sprite.h \
    gpuAPI/newGPU/inner_Poly.h \
    gpuAPI/newGPU/inner_Pixel.h \
    gpuAPI/newGPU/inner_Blit.h.org \
    gpuAPI/newGPU/inner_Blit.h.new \
    gpuAPI/newGPU/inner_Blit.h \
    gpuAPI/newGPU/inner_Blit (copy).h \
    gpuAPI/newGPU/fixed.h \
    new_dynarec/pcsxmem.h \
    new_dynarec/new_dynarec.h \
    new_dynarec/fpu.h \
    new_dynarec/emu_if.h \
    new_dynarec/assem_arm.h \
    sound/cfg.h \
    sound/adsr.h \
    sound/xa.h \
    sound/stdafx.h \
    sound/spu.h \
    sound/reverb.h \
    sound/regs.h \
    sound/registers.h \
    sound/gauss_i.h \
    sound/externals.h \
    sound/dsoundoss.h \
    sound/dma.h \
    sound/psemuxa.h \
    video/inner_Blit.h \
    video/hq3x.h \
    video/hq2x.h \
    video/gpu.h \
    video/fps.h \
    video/externals.h \
    video/draw.h \
    video/cfg.h \
    video/swap.h \
    video/soft.h \
    video/prim.h \
    video/menu.h \
    video/key.h \
    video/interp.h \
    meego/minimal.h

SOURCES += \
    plugins.c \
    misc.c \
    cdriso.c \
    sio.c \
    r3000a.c \
    psxmem.c \
    psxinterpreter.c \
    spu.c \
    socket.c \
    psxhw.c \
    psxhle.c \
    psxdma.c \
    psxcounters.c \
    psxcommon.c \
    psxbios.c \
    ppf.c \
    mdec.c \
    debug.c \
    LnxMain.c \
    cheat.c \
    cdrom.c \
    gte.c \
    disr3000a.c \
    decode_xa.c \
    gpuAPI/gpuAPI.cpp \
    gpuAPI/newGPU/raster_Sprite.cpp \
    gpuAPI/newGPU/raster_Poly.cpp \
    gpuAPI/newGPU/raster_Line.cpp \
    gpuAPI/newGPU/raster_Image.cpp \
    gpuAPI/newGPU/newGPU.cpp \
    gpuAPI/newGPU/inner.cpp \
    gpuAPI/newGPU/fixed.cpp \
    gpuAPI/newGPU/core_Misc.cpp \
    gpuAPI/newGPU/core_Draw.cpp \
    gpuAPI/newGPU/core_Dma.cpp \
    gpuAPI/newGPU/core_Command.cpp \
    new_dynarec/pcsxmem_inline.c \
    new_dynarec/pcsxmem.c \
    new_dynarec/new_dynarec.c \
    new_dynarec/fpu.c \
    new_dynarec/emu_if.c \
    new_dynarec/assem_arm.c \
	sound/nullsnd.c \
    video/zn.c \
	video/fps.c \
    video/draw.c \
    video/cfg.c \
    video/soft.c \
    video/prim.c \
    video/menu.c \
    video/key.c

