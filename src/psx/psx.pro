include(../machine.pri)

DEFINES += DYNAREC

unix {
	qml.path = /opt/emumaster/qml/psx
	qml.files = \
		../../qml/psx/main.qml \
		../../qml/psx/MainPage.qml \
		../../qml/psx/SettingsPage.qml

	gameclassify.path = /usr/share/policy/etc/syspart.conf.d
	gameclassify.files += $${TARGET}.conf

	INSTALLS += qml gameclassify
}

HEADERS += \
    psemu_plugin_defs.h \
    ppf.h \
    plugins.h \
    misc.h \
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
	gpu/fixed.h \
	gpu/raster.h \
	gpu/profiller.h \
	gpu/op_Texture.h \
	gpu/op_Light.h \
	gpu/op_Blend.h \
	gpu/newGPU.h \
	gpu/inner_Sprite.h \
	gpu/inner_Poly.h \
	gpu/inner_Pixel.h \
	gpu/gpuAPI.h \
    sound/xa.h \
    sound/stdafx.h \
    sound/spu.h \
    sound/reverb.h \
    sound/regs.h \
    sound/registers.h \
    sound/psemuxa.h \
    sound/gauss_i.h \
    sound/externals.h \
    sound/dma.h \
	sound/adsr.h \
	machine.h \
    pad.h \
    gpu.h

SOURCES += \
	gpu/inner.cpp \
	gpu/fixed.cpp \
	gpu/core_Misc.cpp \
	gpu/core_Draw.cpp \
	gpu/core_Dma.cpp \
	gpu/core_Command.cpp \
	gpu/raster_Sprite.cpp \
	gpu/raster_Poly.cpp \
	gpu/raster_Line.cpp \
	gpu/raster_Image.cpp \
	gpu/newGPU.cpp \
	gpu/ARM_asm.S \
	gpu/gpuAPI.cpp \
	machine.cpp \
	video_blit.S \
	gte_neon.S \
	new_dynarec/linkage_arm.S \
    sound/xa.cpp \
    sound/spu.cpp \
    sound/reverb.cpp \
    sound/registers.cpp \
    sound/freeze.cpp \
    sound/dma.cpp \
    sound/adsr.cpp \
    plugins.cpp \
    cdriso.cpp \
    cdrom.cpp \
    cheat.cpp \
    debug.cpp \
    decode_xa.cpp \
    disr3000a.cpp \
    gte.cpp \
    LnxMain.cpp \
    mdec.cpp \
    misc.cpp \
    ppf.cpp \
    psxbios.cpp \
    psxcommon.cpp \
    psxcounters.cpp \
    psxdma.cpp \
    psxhle.cpp \
    psxhw.cpp \
    psxinterpreter.cpp \
    psxmem.cpp \
    r3000a.cpp \
    sio.cpp \
    socket.cpp \
    new_dynarec/fpu.c \
    new_dynarec/new_dynarec.c \
	new_dynarec/pcsxmem.c \
    pad.cpp \
    gpu.cpp \
    new_dynarec/emu_if.cpp

OTHER_FILES += \
	new_dynarec/assem_arm.c \
	new_dynarec/pcsxmem_inline.c








