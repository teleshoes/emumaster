include(../nes_mapper.pri)

HEADERS += \
    mapper42.h

SOURCES += \
    mapper42.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper42/lib
    INSTALLS += target
}
