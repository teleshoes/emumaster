include(../nes_mapper.pri)

HEADERS += \
    mapper32.h

SOURCES += \
    mapper32.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper32/lib
    INSTALLS += target
}
