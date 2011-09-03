include(../machine.pri)
LIBS += -lm6502 -lcrc32
DEFINES += NES_PROJECT
QT += opengl declarative

HEADERS += \
    nesdisk.h \
    nespad.h \
    nesmachine.h \
	nescpumapper.h \
    nesmapper.h \
	nesppumapper.h \
    nes_global.h \
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

unix {
	qml.path = /opt/emumaster/qml/nes
	qml.files = \
		../../qml/nes/main.qml \
		../../qml/nes/MainPage.qml \
		../../qml/nes/VideoPage.qml \
		../../qml/nes/AudioPage.qml \
		../../qml/nes/InputPage.qml \
		../../qml/nes/StatePage.qml \
		../../qml/nes/CheatPage.qml \
		../../qml/nes/MachineInfoSheet.qml
	INSTALLS += qml
}
