include(../nes_mapper.pri)

HEADERS += \
    mapper45.h

SOURCES += \
    mapper45.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper45/lib
    INSTALLS += target
}
