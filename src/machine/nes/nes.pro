include(../machine.pri)
LIBS += -lmemory8 -lm6502 -lhostaudio -lcrc32
DEFINES += NES_PROJECT
QT += opengl multimedia

HEADERS += \
    nesdisk.h \
    nespad.h \
    nesmachine.h \
	nescpumapper.h \
    nesmapper.h \
	nesppumapper.h \
    nes_global.h \
    nesmachineview.h \
	nesapu.h \
    nesapunoisechannel.h \
    nesapuchannel.h \
    nesapurectanglechannel.h \
    nesaputrianglechannel.h \
    nesapudmchannel.h \
    nescpu.h \
    nesppu.h \
    nesppusprite.h \
    nesppuregisters.h \
    nesppupalette.h \
    nesdisk_p.h \
    nesmappereeprom.h

SOURCES += \
    nesdisk.cpp \
    nespad.cpp \
    nesmachine.cpp \
	nescpumapper.cpp \
    nesmapper.cpp \
	nesppumapper.cpp \
    nesmachineview.cpp \
	nesapu.cpp \
    nesapunoisechannel.cpp \
    nesapuchannel.cpp \
    nesapurectanglechannel.cpp \
    nesaputrianglechannel.cpp \
    nesapudmchannel.cpp \
    nescpu.cpp \
    nesppu.cpp \
    nesppuregisters.cpp \
    nesppupalette.cpp \
    nesdisk_patch.cpp \
    nesmappereeprom.cpp
