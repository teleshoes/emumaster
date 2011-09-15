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
    machine.h \
    cpu.h \
    common.h \
    cheats.h \
    mem.h \
    spu.h \
    gpu.h

SOURCES += \
    cheats.cpp \
    cpu.cpp \
    cpu_threaded.c \
    machine.cpp \
    mem.cpp \
    spu.cpp \
    gpu.cpp

QMAKE_CFLAGS += -std=c99

unix {
	qml.path = /opt/emumaster/qml/gba
	qml.files = \
		../../qml/gba/main.qml \
		../../qml/gba/MainPage.qml \
		../../qml/gba/SettingsPage.qml

	game_config.path = /opt/emumaster/data
	game_config.files = gba_game_config.txt

	gameclassify.path = /usr/share/policy/etc/syspart.conf.d
	gameclassify.files += $${TARGET}.conf

	INSTALLS += qml game_config gameclassify
}



































































































