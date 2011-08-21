include(../nes_mapper.pri)

HEADERS += \
    mapper67.h

SOURCES += \
    mapper67.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper67/lib
    INSTALLS += target
}
