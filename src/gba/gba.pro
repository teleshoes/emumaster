include(../machine.pri)

DEFINES += USE_ASM_VIDEO_EXPAND

OTHER_FILES += \
    game_config.txt \
    x86/x86_stub.S \
	x86/Makefile

contains(MEEGO_EDITION,harmattan): {
	SOURCES += \
		arm/video_blend.S \
		arm/arm_stub.S
}

HEADERS += \
    cpu.h \
    common.h \
    cheats.h \
    video.h \
    sound.h \
    memory.h \
    main.h \
    x86/x86_emit.h \
    arm/arm_emit.h \
    arm/arm_dpimacros.h \
    arm/arm_codegen.h \
    gbamachine.h \
    gbapad.h

SOURCES += \
    cpu_threaded.c \
    cpu.c \
    cheats.c \
    video.c \
    sound.c \
    memory.c \
    main.c \
    input.c \
    gbamachine.cpp \
    gbapad.cpp

unix {
	qml.path = /opt/emumaster/qml/gba
	qml.files = \
		../../../qml/gba/main.qml \
		../../../qml/gba/MainPage.qml \
		../../../qml/gba/VideoPage.qml \
		../../../qml/gba/AudioPage.qml \
		../../../qml/gba/InputPage.qml \
		../../../qml/gba/StatePage.qml \
		../../../qml/gba/CheatPage.qml \
		../../../qml/gba/MachineInfoSheet.qml

	game_config.path = /home/user/MyDocs/emumaster/gba
	game_config.files = game_config.txt

	INSTALLS += qml
}

