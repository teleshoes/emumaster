include(../nes_mapper.pri)

HEADERS += \
    mapper11.h

SOURCES += \
    mapper11.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper11/lib
    INSTALLS += target
}
