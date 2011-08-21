include(../nes_mapper.pri)

HEADERS += \
    mapper41.h

SOURCES += \
    mapper41.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper41/lib
    INSTALLS += target
}
