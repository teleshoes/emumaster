include(../nes_mapper.pri)

HEADERS += \
    mapper9.h

SOURCES += \
    mapper9.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper9/lib
    INSTALLS += target
}
