include(../nes_mapper.pri)

HEADERS += \
    mapper10.h

SOURCES += \
    mapper10.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper10/lib
    INSTALLS += target
}
