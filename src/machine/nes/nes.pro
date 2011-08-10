include(../machine.pri)
LIBS += -lnes2c0x -lmemory8 -lm6502 -lhostaudio
DEFINES += NES_PROJECT
QT += opengl multimedia

HEADERS += \
    nesdisk.h \
    nespad.h \
    nesmachine.h \
    nescpumemorymapper.h \
    nesmapper.h \
    nesppumemorymapper.h \
    nes_global.h \
    nesmachineview.h \
	nesapu.h \
    nesapunoisechannel.h \
    nesapuchannel.h \
    nesapurectanglechannel.h \
    nesaputrianglechannel.h \
    nesapudmchannel.h \
    nescpu.h \
    nesppu.h

SOURCES += \
    nesdisk.cpp \
    nespad.cpp \
    nesmachine.cpp \
    nescpumemorymapper.cpp \
    nesmapper.cpp \
    nesppumemorymapper.cpp \
    nesmachineview.cpp \
	nesapu.cpp \
    nesapunoisechannel.cpp \
    nesapuchannel.cpp \
    nesapurectanglechannel.cpp \
    nesaputrianglechannel.cpp \
    nesapudmchannel.cpp \
    nescpu.cpp \
    nesppu.cpp
