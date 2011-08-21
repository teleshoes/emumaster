include(../nes_mapper.pri)

HEADERS += \
    mapper61.h

SOURCES += \
    mapper61.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper61/lib
    INSTALLS += target
}
