include(../nes_mapper.pri)

HEADERS += \
    mapper51.h

SOURCES += \
    mapper51.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper51/lib
    INSTALLS += target
}
