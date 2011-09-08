include(../machine.pri)

DEFINES += DYNAREC

unix {
	qml.path = /opt/emumaster/qml/psx
	qml.files = \
		../../qml/psx/main.qml \
		../../qml/psx/MainPage.qml \
		../../qml/psx/SettingsPage.qml

	INSTALLS += qml
}

ASM_SOURCES += \
	gte_neon.S \
	new_dynarec/linkage_arm.S

SOURCES += $$ASM_SOURCES \
    gpuAPI/newGPU/inner.cpp \
    gpuAPI/newGPU/fixed.cpp \
    gpuAPI/newGPU/core_Misc.cpp \
    gpuAPI/newGPU/core_Draw.cpp \
    gpuAPI/newGPU/core_Dma.cpp \
    gpuAPI/newGPU/core_Command.cpp \
    gpuAPI/newGPU/raster_Sprite.cpp \
    gpuAPI/newGPU/raster_Poly.cpp \
    gpuAPI/newGPU/raster_Line.cpp \
    gpuAPI/newGPU/raster_Image.cpp \
	gpuAPI/newGPU/newGPU.cpp \
	gpuAPI/newGPU/ARM_asm.S \
    gpuAPI/gpuAPI.cpp \
    sound/xa.c \
    sound/spu.c \
    sound/reverb.c \
    sound/registers.c \
    sound/pulseaudio.c \
    sound/nullsnd.c \
    sound/freeze.c \
    sound/dma.c \
    sound/cfg.c \
    sound/adsr.c \
    android/minimal.c \
	android/emulator.cpp \
	android/video_blit.S

HEADERS += \
    psemu_plugin_defs.h \
    ppf.h \
    plugins.h \
    misc.h \
    minimal.h \
    mdec.h \
    gte_neon.h \
    gte_divider.h \
    gte.h \
    decode_xa.h \
    debug.h \
    config.h \
    coff.h \
    cheat.h \
    cdrom.h \
    cdriso.h \
    system.h \
    spu.h \
    socket.h \
    sjisfont.h \
    sio.h \
    r3000a.h \
    psxmem.h \
    psxhw.h \
    psxhle.h \
    psxdma.h \
    psxcounters.h \
    psxcommon.h \
    psxbios.h \
    new_dynarec/pcsxmem.h \
    new_dynarec/new_dynarec.h \
    new_dynarec/fpu.h \
    new_dynarec/emu_if.h \
    new_dynarec/assem_arm.h \
    gpuAPI/newGPU/inner_Blit.h \
    gpuAPI/newGPU/inner_Blit (copy).h \
    gpuAPI/newGPU/fixed.h \
    gpuAPI/newGPU/raster.h \
    gpuAPI/newGPU/profiller.h \
    gpuAPI/newGPU/op_Texture.h \
    gpuAPI/newGPU/op_Light.h \
    gpuAPI/newGPU/op_Blend.h \
    gpuAPI/newGPU/newGPU.h \
    gpuAPI/newGPU/inner_Sprite.h \
    gpuAPI/newGPU/inner_Poly.h \
    gpuAPI/newGPU/inner_Pixel.h \
    gpuAPI/gpuAPI.h \
    sound/xa.h \
    sound/stdafx.h \
    sound/spu.h \
    sound/reverb.h \
    sound/regs.h \
    sound/registers.h \
    sound/psemuxa.h \
    sound/gauss_i.h \
    sound/externals.h \
    sound/dsoundoss.h \
    sound/dma.h \
    sound/cfg.h \
    sound/adsr.h \
    android/minimal.h \
    android/emulator.h

SOURCES += \
    ppf.c \
    plugins.c \
    misc.c \
    mdec.c \
    LnxMain.c \
    gte.c \
    disr3000a.c \
    decode_xa.c \
    debug.c \
    cheat.c \
    cdrom.c \
    cdriso.c \
    socket.c \
    sio.c \
    r3000a.c \
    psxmem.c \
    psxinterpreter.c \
    psxhw.c \
    psxhle.c \
    psxdma.c \
    psxcounters.c \
    psxcommon.c \
    psxbios.c \
    new_dynarec/pcsxmem_inline.c \
    new_dynarec/pcsxmem.c \
    new_dynarec/new_dynarec.c \
    new_dynarec/fpu.c \
    new_dynarec/emu_if.c \
    new_dynarec/assem_arm.c

