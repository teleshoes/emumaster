include(../nes_mapper.pri)

HEADERS += \
    mapper50.h

SOURCES += \
    mapper50.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper50/lib
    INSTALLS += target
}
