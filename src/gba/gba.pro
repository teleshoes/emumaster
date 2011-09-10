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
    gbamachine.h \
    gbapad.h \
    gbasound.h \
    gbamemory.h \
    gbavideo.h \
    gbacpu.h \
    gbacommon.h \
    gbacheats.h

SOURCES += \
    gbamachine.cpp \
    gbapad.cpp \
    gbavideo.cpp \
    gbamemory.cpp \
    gbasound.cpp \
    gbacpu_threaded.c \
    gbacpu.cpp \
    gbacheats.cpp

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


















































