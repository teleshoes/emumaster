include(../nes_mapper.pri)

HEADERS += \
    mapper66.h

SOURCES += \
    mapper66.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper66/lib
    INSTALLS += target
}
