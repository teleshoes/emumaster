include(../nes_mapper.pri)

HEADERS += \
    mapper18.h

SOURCES += \
    mapper18.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper18/lib
    INSTALLS += target
}
