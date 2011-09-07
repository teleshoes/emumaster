include(../machine.pri)

unix {
	qml.path = /opt/emumaster/qml/snes
	qml.files = \
		../../qml/snes/main.qml \
		../../qml/snes/MainPage.qml \
		../../qml/snes/SettingsPage.qml

	INSTALLS += qml game_config
}

OTHER_FILES += \
    os9x_65c816.S \
    spc_decode.S \
    spc700a.S \
    m3d_func.S \
    generatePPUasm/ppuasmfunc16.S

HEADERS += \
    sdd1emu.h \
    snapshot.h \
    port.h \
    fxinst.h \
    fxemu.h \
    os9x_asm_cpu.h \
    memmap.h \
    ppu.h \
    tile.h \
    srtc.h \
    spc700.h \
    snes9x.h \
    soundux.h \
    seta.h \
    gfx.h \
    dma.h \
    cpuexec.h \
    c4.h \
    apumem.h \
    apu.h \
    sdd1.h

SOURCES += \
    sdd1emu.cpp \
    snapshot.cpp \
    fxemu.cpp \
    fxinst.cpp \
    os9x_asm_cpu.cpp \
    memmap.cpp \
    ppu.cpp \
    globals.cpp \
    tile.cpp \
    srtc.cpp \
    spc700.cpp \
    soundux.cpp \
    seta010.cpp \
    seta011.cpp \
    seta018.cpp \
    seta.cpp \
    gfx.cpp \
    dma.cpp \
    data.cpp \
    cpuexec.cpp \
    cpu.cpp \
    clip.cpp \
    c4emu.cpp \
    c4.cpp \
    apu.cpp \
    sdd1.cpp







