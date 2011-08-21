include(../nes_mapper.pri)

HEADERS += \
    mapper26.h

SOURCES += \
    mapper26.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper26/lib
    INSTALLS += target
}
