include(../nes_mapper.pri)

HEADERS += \
    mapper71.h

SOURCES += \
    mapper71.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper71/lib
    INSTALLS += target
}
