include(../nes_mapper.pri)

HEADERS += \
    mapper19.h

SOURCES += \
    mapper19.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper19/lib
    INSTALLS += target
}
