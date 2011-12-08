DESTDIR = ../../bin
INCLUDEPATH += ../../include
LIBS += -L../../lib -lbase
QT += opengl declarative network

SOURCES += \
    main.cpp \
    diskgallery.cpp \
    diskimageprovider.cpp \
    disklistmodel.cpp \
    disklistmodel_fav.cpp \
    disklistmodel_icon.cpp

HEADERS += \
    diskgallery.h \
    diskimageprovider.h \
    disklistmodel.h

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
		../../qml/gallery/AboutPage.qml \
		../../qml/gallery/CoverSelectorPage.qml \
		../../qml/gallery/HomeScreenIconSheet.qml \
		../../qml/gallery/CollectionMenuPage.qml \
		../../qml/gallery/CollectionTypeButton.qml \
		../../qml/gallery/GalleryPage.qml \
		../../qml/gallery/GlobalSettings.qml

	qmlimg.path = /opt/emumaster/qml/img
	qmlimg.files = \
		../../qml/img/collection-gba.png \
		../../qml/img/collection-nes.png \
		../../qml/img/collection-snes.png \
		../../qml/img/collection-psx.png \
		../../qml/img/collection-amiga.png \
		../../qml/img/collection-fav.png \
		../../qml/img/alpha-overlay.png \
		../../qml/img/btn_donateCC_LG.png \
		../../qml/img/wiki.png \

	datafiles.path = /opt/emumaster/data
	datafiles.files = \
		../../data/icon_mask.png \
		../../data/icon_overlay.png

	INSTALLS += target qml datafiles qmlimg
}

contains(MEEGO_EDITION,harmattan) {
	icon.files = diskgallery.png
	icon.path = /opt/emumaster/data
    INSTALLS += icon
}

contains(MEEGO_EDITION,harmattan) {
    desktopfile.files = $${TARGET}.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile
}

maemo5 {
	icon.files = diskgallery.png
	icon.path = /opt/emumaster/data
	INSTALLS += icon

	desktopfile.files = $${TARGET}.desktop
	desktopfile.path = /usr/share/applications/hildon
	INSTALLS += desktopfile
}




























