include(../nes_mapper.pri)

HEADERS += \
    mapper62.h

SOURCES += \
    mapper62.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper62/lib
    INSTALLS += target
}
