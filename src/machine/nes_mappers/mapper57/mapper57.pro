include(../nes_mapper.pri)

HEADERS += \
    mapper57.h

SOURCES += \
    mapper57.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper57/lib
    INSTALLS += target
}
