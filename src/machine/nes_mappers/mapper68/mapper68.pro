include(../nes_mapper.pri)

HEADERS += \
    mapper68.h

SOURCES += \
    mapper68.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/mapper68/lib
    INSTALLS += target
}
