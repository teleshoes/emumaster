include(../nes_mapper.pri)

HEADERS += \
    mapper15.h

SOURCES += \
    mapper15.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper15/lib
    INSTALLS += target
}
