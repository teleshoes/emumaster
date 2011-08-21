include(../nes_mapper.pri)

HEADERS += \
    mapper44.h

SOURCES += \
    mapper44.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper44/lib
    INSTALLS += target
}
