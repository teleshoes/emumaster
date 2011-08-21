include(../nes_mapper.pri)

HEADERS += \
    mapper40.h

SOURCES += \
    mapper40.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper40/lib
    INSTALLS += target
}
