include(../nes_mapper.pri)

HEADERS += \
    mapper4.h

SOURCES += \
    mapper4.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper4/lib
    INSTALLS += target
}
