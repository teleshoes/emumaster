include(../nes_mapper.pri)

HEADERS += \
    mapper16.h

SOURCES += \
    mapper16.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper16/lib
    INSTALLS += target
}
