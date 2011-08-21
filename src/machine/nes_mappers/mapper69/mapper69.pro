include(../nes_mapper.pri)

HEADERS += \
    mapper69.h

SOURCES += \
    mapper69.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper69/lib
    INSTALLS += target
}
