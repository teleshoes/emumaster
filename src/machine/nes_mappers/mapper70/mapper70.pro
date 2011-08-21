include(../nes_mapper.pri)

HEADERS += \
    mapper70.h

SOURCES += \
    mapper70.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper70/lib
    INSTALLS += target
}
