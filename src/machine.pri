DESTDIR = ../../bin
LIBS += -L../../lib -lbase
INCLUDEPATH += ../../include
QT += opengl

contains(CONFIG,release) {
	QMAKE_CXXFLAGS += \
		-march=armv7-a \
		-mcpu=cortex-a8 \
		-mtune=cortex-a8 \
		-mfpu=neon -O3 \
		-ffast-math \
		-ftemplate-depth-36 \
		-fstrict-aliasing \
		-mstructure-size-boundary=32 \
		-falign-functions=32
}

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
