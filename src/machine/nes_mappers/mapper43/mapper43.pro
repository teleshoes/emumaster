include(../nes_mapper.pri)

HEADERS += \
    mapper43.h

SOURCES += \
    mapper43.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper43/lib
    INSTALLS += target
}
