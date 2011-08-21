include(../nes_mapper.pri)

HEADERS += \
    mapper60.h

SOURCES += \
    mapper60.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper60/lib
    INSTALLS += target
}
