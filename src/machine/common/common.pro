include(../machine.pri)
TARGET = machine_common
QT += declarative multimedia opengl
LIBS += -lpulse-simple -lgamegeniecode

DEFINES += MACHINE_COMMON_PROJECT

HEADERS += \
    machineview.h \
    machine_common_global.h \
    imachine.h \
    machinethread.h \
    hostaudio.h \
    hostvideo.h \
    hostinput.h \
    machineimageprovider.h \
    machinestatelistmodel.h \
    gamegeniecodelistmodel.h

SOURCES += \
    machineview.cpp \
    imachine.cpp \
    machinethread.cpp \
    hostaudio.cpp \
    hostvideo.cpp \
    hostinput.cpp \
    machineimageprovider.cpp \
    machinestatelistmodel.cpp \
    gamegeniecodelistmodel.cpp




