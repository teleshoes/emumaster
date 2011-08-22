TEMPLATE = lib
TARGET = machine_common
DESTDIR = ../../../lib
QT += declarative multimedia opengl
LIBS += -lpulse-simple

DEFINES += MACHINE_COMMON_PROJECT

HEADERS += \
    machineview.h \
    machine_common_global.h \
    imachine.h \
    machinethread.h \
    hostaudio.h \
    hostvideo.h \
    hostinput.h \
    machineimageprovider.h

SOURCES += \
    machineview.cpp \
    imachine.cpp \
    machinethread.cpp \
    hostaudio.cpp \
    hostvideo.cpp \
    hostinput.cpp \
    machineimageprovider.cpp
