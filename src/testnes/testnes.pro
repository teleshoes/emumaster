DESTDIR = ../../bin
INCLUDEPATH += ../machine/nes ../../include
LIBS += -L../../lib -lnes -lmachine_common -lpulse-simple
QT += opengl

SOURCES += \
    main.cpp

unix:!symbian:!maemo5 {
    target.path = /opt/testnes/bin
    INSTALLS += target
}
