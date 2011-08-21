include(../nes_mapper.pri)

HEADERS += \
    mapper48.h

SOURCES += \
    mapper48.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper48/lib
    INSTALLS += target
}
