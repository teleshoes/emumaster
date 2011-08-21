include(../nes_mapper.pri)

HEADERS += \
    mapper7.h

SOURCES += \
    mapper7.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper7/lib
    INSTALLS += target
}
