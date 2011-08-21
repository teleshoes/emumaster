include(../nes_mapper.pri)

HEADERS += \
    mapper64.h

SOURCES += \
    mapper64.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper64/lib
    INSTALLS += target
}
