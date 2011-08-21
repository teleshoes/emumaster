include(../nes_mapper.pri)

HEADERS += \
    mapper27.h

SOURCES += \
    mapper27.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper27/lib
    INSTALLS += target
}
