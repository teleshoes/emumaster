include(../nes_mapper.pri)

HEADERS += \
    mapper21.h

SOURCES += \
    mapper21.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper21/lib
    INSTALLS += target
}
