include(../nes_mapper.pri)

HEADERS += \
    mapper33.h

SOURCES += \
    mapper33.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper33/lib
    INSTALLS += target
}
