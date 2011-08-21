include(../nes_mapper.pri)

HEADERS += \
    mapper5.h

SOURCES += \
    mapper5.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper5/lib
    INSTALLS += target
}
