include(../nes_mapper.pri)

HEADERS += \
    mapper34.h

SOURCES += \
    mapper34.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper34/lib
    INSTALLS += target
}
