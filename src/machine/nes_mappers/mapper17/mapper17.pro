include(../nes_mapper.pri)

HEADERS += \
    mapper17.h

SOURCES += \
    mapper17.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper17/lib
    INSTALLS += target
}
