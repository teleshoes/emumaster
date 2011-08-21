include(../nes_mapper.pri)

HEADERS += \
    mapper13.h

SOURCES += \
    mapper13.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper13/lib
    INSTALLS += target
}
