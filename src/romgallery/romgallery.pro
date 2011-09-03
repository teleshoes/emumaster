DESTDIR = ../../bin
INCLUDEPATH += ../../include
LIBS += -L../../lib -lmachine_common
QT += opengl declarative

SOURCES += \
    main.cpp \
    romimageprovider.cpp \
    romgallery.cpp \
    romlistmodel.cpp

HEADERS += \
    romimageprovider.h \
    romgallery.h \
    romlistmodel.h

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
	qml.path = /opt/emumaster/qml/gallery
	qml.files = \
		../../qml/gallery/main.qml \
		../../qml/gallery/RomChoosePage.qml \
		../../qml/gallery/GalleryPage.qml \
		../../qml/gallery/ListPage.qml \
		../../qml/gallery/AboutSheet.qml

	datafiles.path = /opt/emumaster/data
	datafiles.files = \
		../../data/icon_mask.png \
		../../data/icon_overlay.png

	INSTALLS += target qml datafiles
}

contains(MEEGO_EDITION,harmattan) {
    icon.files = romgallery.png
	icon.path = /opt/emumaster/data
    INSTALLS += icon
}

contains(MEEGO_EDITION,harmattan) {
    desktopfile.files = $${TARGET}.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile
}
