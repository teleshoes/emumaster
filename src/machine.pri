DESTDIR = ../../bin
LIBS += -L../../lib -lbase
INCLUDEPATH += ../../include
QT += opengl

linux-g++-maemo {
	MEEGO_VERSION_MAJOR     = 1
	MEEGO_VERSION_MINOR     = 2
	MEEGO_VERSION_PATCH     = 0
	MEEGO_EDITION           = harmattan
	DEFINES += MEEGO_EDITION_HARMATTAN
}

unix {
	QMAKE_LFLAGS += -Wl,--rpath,/opt/emumaster/bin -Wl,--rpath,/opt/emumaster/lib
	target.path = /opt/emumaster/bin
	INSTALLS += target
}
