include(../../component.pri)

DEFINES += NES2C0X_PROJECT

HEADERS += \
    sprite.h \
    registers.h \
    ppu.h \
    palette.h \
    nes2c0x_global.h

SOURCES += \
    rendering.cpp \
    registers.cpp \
    ppu.cpp \
    palette.cpp
