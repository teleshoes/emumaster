include(../common.pri)

DEFINES += M6502_PROJECT

HEADERS += \
    m6502.h \
    m6502_global.h

SOURCES += \
    opcodes.cpp \
    cpu.cpp \
    tables.cpp
