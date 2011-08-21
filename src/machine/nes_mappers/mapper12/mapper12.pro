include(../nes_mapper.pri)

HEADERS += \
    mapper12.h

SOURCES += \
    mapper12.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper12/lib
    INSTALLS += target
}
