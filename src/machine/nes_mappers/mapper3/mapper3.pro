include(../nes_mapper.pri)

HEADERS += \
    mapper3.h

SOURCES += \
    mapper3.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper3/lib
    INSTALLS += target
}
