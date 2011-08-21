include(../nes_mapper.pri)

HEADERS += \
    mapper58.h

SOURCES += \
    mapper58.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper58/lib
    INSTALLS += target
}
