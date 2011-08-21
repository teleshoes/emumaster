include(../nes_mapper.pri)

HEADERS += \
    mapper24.h

SOURCES += \
    mapper24.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper24/lib
    INSTALLS += target
}
