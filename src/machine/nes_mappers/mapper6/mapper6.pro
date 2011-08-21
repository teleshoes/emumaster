include(../nes_mapper.pri)

HEADERS += \
    mapper6.h

SOURCES += \
    mapper6.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper6/lib
    INSTALLS += target
}
