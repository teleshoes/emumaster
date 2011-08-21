include(../nes_mapper.pri)

HEADERS += \
    mapper65.h

SOURCES += \
    mapper65.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper65/lib
    INSTALLS += target
}
