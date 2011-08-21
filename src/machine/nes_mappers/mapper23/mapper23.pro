include(../nes_mapper.pri)

HEADERS += \
    mapper23.h

SOURCES += \
    mapper23.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper23/lib
    INSTALLS += target
}
