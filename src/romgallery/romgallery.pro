DESTDIR = ../../bin
INCLUDEPATH += ../../include
LIBS += -L../../lib -lbase
QT += opengl declarative network

SOURCES += \
    main.cpp \
    romimageprovider.cpp \
    romgallery.cpp \
    romlistmodel.cpp

HEADERS += \
    romimageprovider.h \
    romgallery.h \
    romlistmodel.h

linux-g++-maemo {
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
		../../qml/gallery/AboutSheet.qml \
		../../qml/gallery/HomeScreenIconSheet.qml \
		../../qml/gallery/MachineTypePage.qml \
		../../qml/gallery/MachineTypeButton.qml \
		../../qml/gallery/CoverSelectorSheet.qml

	qmlimg.path = /opt/emumaster/qml/img
	qmlimg.files = \
		../../qml/img/machine-gba.png \
		../../qml/img/machine-nes.png \
		../../qml/img/machine-snes.png \
		../../qml/img/machine-psx.png \

	datafiles.path = /opt/emumaster/data
	datafiles.files = \
		../../data/icon_mask.png \
		../../data/icon_overlay.png

	INSTALLS += target qml datafiles qmlimg
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


