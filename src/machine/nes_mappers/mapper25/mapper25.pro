include(../nes_mapper.pri)

HEADERS += \
    mapper25.h

SOURCES += \
    mapper25.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper25/lib
    INSTALLS += target
}
