include(../nes_mapper.pri)

HEADERS += \
    mapper2.h

SOURCES += \
    mapper2.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper2/lib
    INSTALLS += target
}
