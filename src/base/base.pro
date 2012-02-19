TEMPLATE = lib
DESTDIR = ../../lib
INCLUDEPATH += ../../include
QT += declarative opengl network
LIBS += -L../../lib -lpulse -lsixaxisclient
CONFIG += mobility
MOBILITY += sensors

linux-g++-maemo {
    MEEGO_VERSION_MAJOR     = 1
    MEEGO_VERSION_MINOR     = 2
    MEEGO_VERSION_PATCH     = 0
    MEEGO_EDITION           = harmattan
    DEFINES += MEEGO_EDITION_HARMATTAN
}

unix {
    QMAKE_LFLAGS += -Wl,--rpath,/opt/emumaster/lib
    target.path = /opt/emumaster/lib
    INSTALLS += target
}

DEFINES += BASE_PROJECT

HEADERS += \
    hostaudio.h \
    hostvideo.h \
    hostinput.h \
	base_global.h \
    pathmanager.h \
    statelistmodel.h \
	stateimageprovider.h \
    configuration.h \
    hostinputdevice.h \
    accelinputdevice.h \
    touchinputdevice.h \
    sixaxisinputdevice.h \
    keybinputdevice.h \
    crc32.h \
    emuview.h \
    emuthread.h \
    emu.h \
    emuinput.h \
    stringlistproxy.h

SOURCES += \
    hostaudio.cpp \
    hostvideo.cpp \
    hostinput.cpp \
    pathmanager.cpp \
    statelistmodel.cpp \
	stateimageprovider.cpp \
    configuration.cpp \
    hostinputdevice.cpp \
    accelinputdevice.cpp \
    touchinputdevice.cpp \
    sixaxisinputdevice.cpp \
    keybinputdevice.cpp \
    crc32.cpp \
    emuview.cpp \
    emuthread.cpp \
    emu.cpp \
    emuinput.cpp \
    stringlistproxy.cpp

unix {
	qml.path = /opt/emumaster/qml/base
	qml.files = \
        ../../qml/base/main.qml \
        ../../qml/base/error.qml \
        ../../qml/base/SettingsPage.qml \
        ../../qml/base/SettingsSwitchItem.qml \
        ../../qml/base/NesSettingsPage.qml \
        ../../qml/base/NesCheats.qml \
        ../../qml/base/SnesSettingsPage.qml \
        ../../qml/base/GbaSettingsPage.qml \
        ../../qml/base/PsxSettingsPage.qml \
        ../../qml/base/AmigaSettingsPage.qml \
        ../../qml/base/PicoSettingsPage.qml \
		\
        ../../qml/base/EMButtonOption.qml \
        ../../qml/base/EMSwitchOption.qml \
        ../../qml/base/ImageListViewDelegate.qml \
        ../../qml/base/MySectionScroller.js \
        ../../qml/base/MySectionScroller.qml \
        ../../qml/base/MyListDelegate.qml \
        ../../qml/base/SectionScrollerLabel.qml \
        ../../qml/base/SectionSeperator.qml \
        ../../qml/base/constants.js \
        ../../qml/base/utils.js

	pad.path = /opt/emumaster/data
	pad.files = \
        ../../data/pad.png

	qmlimg.path = /opt/emumaster/qml/img
	qmlimg.files = \
        ../../qml/img/input-accel.png \
        ../../qml/img/input-keyb.png \
        ../../qml/img/input-sixaxis.png \
        ../../qml/img/input-touch.png

	INSTALLS += qml qmlimg pad
}

contains(MEEGO_EDITION, harmattan) {
	CONFIG += qmsystem2
}
