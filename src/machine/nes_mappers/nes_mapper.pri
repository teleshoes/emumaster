TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../../../../lib/nes_mapper
LIBS += -L../../../../lib -lnes
INCLUDEPATH += ../../nes ../../../../include

exists($$QMAKE_INCDIR_QT"/../qmsystem2/qmkeys.h"):!contains(MEEGO_EDITION,harmattan): {
	MEEGO_VERSION_MAJOR     = 1
	MEEGO_VERSION_MINOR     = 2
	MEEGO_VERSION_PATCH     = 0
	MEEGO_EDITION           = harmattan
	DEFINES += MEEGO_EDITION_HARMATTAN
}

unix {
	QMAKE_LFLAGS += -Wl,--rpath,/opt/emumaster/lib/nes_mapper:/opt/emumaster/lib
	target.path = /opt/emumaster/lib/nes_mapper
	INSTALLS += target
}
