include(../nes_mapper.pri)

HEADERS += \
    mapper47.h

SOURCES += \
    mapper47.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper47/lib
    INSTALLS += target
}
