include(../machine.pri)

DEFINES += USE_ASM_VIDEO_EXPAND
QMAKE_CFLAGS += -std=c99

contains(MEEGO_EDITION,harmattan): {
	SOURCES += \
		arm/video_blend.S \
		arm/arm_stub.S
}

HEADERS += \
    arm/arm_emit.h \
    arm/arm_dpimacros.h \
    arm/arm_codegen.h \
    video.h \
    sound.h \
    pad.h \
    memory.h \
    machine.h \
    cpu.h \
    common.h \
    cheats.h

SOURCES += \
    cheats.cpp \
    cpu.cpp \
    cpu_threaded.c \
    machine.cpp \
    memory.cpp \
    pad.cpp \
    sound.cpp \
    video.cpp

QMAKE_CFLAGS += -std=c99

unix {
	qml.path = /opt/emumaster/qml/gba
	qml.files = \
		../../qml/gba/main.qml \
		../../qml/gba/MainPage.qml \
		../../qml/gba/SettingsPage.qml

	game_config.path = /opt/emumaster/data
	game_config.files = gba_game_config.txt

	INSTALLS += qml game_config
}




















































































