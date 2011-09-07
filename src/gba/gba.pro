include(../machine.pri)

DEFINES += USE_ASM_VIDEO_EXPAND

OTHER_FILES += \
	x86/x86_stub.S

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
    gbamachine.cpp \
    gbapad.cpp


unix {
	qml.path = /opt/emumaster/qml/gba
	qml.files = \
		../../qml/gba/main.qml \
		../../qml/gba/MainPage.qml \
		../../qml/gba/SettingsPage.qml

	game_config.path = /opt/emumaster/data
	game_config.files = game_config.txt

	INSTALLS += qml game_config
}


