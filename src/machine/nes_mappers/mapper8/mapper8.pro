include(../nes_mapper.pri)

HEADERS += \
    mapper8.h

SOURCES += \
    mapper8.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper8/lib
    INSTALLS += target
}
