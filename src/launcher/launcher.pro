DESTDIR = ../../bin
INCLUDEPATH += ../../include
LIBS += -L../../lib -lmachine_common
QT += opengl

SOURCES += \
	main.cpp

exists($$QMAKE_INCDIR_QT"/../qmsystem2/qmkeys.h"):!contains(MEEGO_EDITION,harmattan): {
	MEEGO_VERSION_MAJOR     = 1
	MEEGO_VERSION_MINOR     = 2
	MEEGO_VERSION_PATCH     = 0
	MEEGO_EDITION           = harmattan
	DEFINES += MEEGO_EDITION_HARMATTAN
}

unix {
	QMAKE_LFLAGS += -Wl,--rpath-link,../../lib -Wl,--rpath,/opt/emumaster/lib
	target.path = /opt/emumaster/bin
    INSTALLS += target
}
