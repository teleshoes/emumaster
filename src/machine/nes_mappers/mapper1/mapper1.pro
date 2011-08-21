include(../nes_mapper.pri)

HEADERS += \
    mapper1.h

SOURCES += \
    mapper1.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper1/lib
    INSTALLS += target
}
