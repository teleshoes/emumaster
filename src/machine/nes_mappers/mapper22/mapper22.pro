include(../nes_mapper.pri)

HEADERS += \
    mapper22.h

SOURCES += \
    mapper22.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper22/lib
    INSTALLS += target
}
