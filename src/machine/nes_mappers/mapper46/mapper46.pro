include(../nes_mapper.pri)

HEADERS += \
    mapper46.h

SOURCES += \
    mapper46.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper46/lib
    INSTALLS += target
}
