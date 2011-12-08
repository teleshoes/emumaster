include(../common.pri)
QT += declarative opengl network
LIBS += -L../../../lib -lpulse -lsixaxisclient
CONFIG += mobility
MOBILITY += sensors

DEFINES += BASE_PROJECT

HEADERS += \
    machineview.h \
    imachine.h \
    machinethread.h \
    hostaudio.h \
    hostvideo.h \
    hostinput.h \
    settingsview.h \
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
    crc32.h

SOURCES += \
    machineview.cpp \
    imachine.cpp \
    machinethread.cpp \
    hostaudio.cpp \
    hostvideo.cpp \
    hostinput.cpp \
	settingsview.cpp \
    pathmanager.cpp \
    statelistmodel.cpp \
	stateimageprovider.cpp \
    configuration.cpp \
    hostinputdevice.cpp \
    accelinputdevice.cpp \
    touchinputdevice.cpp \
    sixaxisinputdevice.cpp \
    keybinputdevice.cpp \
    crc32.cpp

unix {
	qml.path = /opt/emumaster/qml/base
	qml.files = \
		../../../qml/base/main.qml \
		../../../qml/base/error.qml \
		../../../qml/base/SettingsPage.qml \
		../../../qml/base/NesCheatPage.qml \
		\
		../../../qml/base/EMButtonOption.qml \
		../../../qml/base/EMSwitchOption.qml \
		../../../qml/base/ImageListViewDelegate.qml \
		../../../qml/base/MySectionScroller.js \
		../../../qml/base/MySectionScroller.qml \
		../../../qml/base/MyListDelegate.qml \
		../../../qml/base/SectionScrollerLabel.qml \
		../../../qml/base/SectionSeperator.qml \
		../../../qml/base/constants.js \
		../../../qml/base/utils.js \

	pad.path = /opt/emumaster/data
	pad.files = \
		../../../data/pad.png

	qmlimg.path = /opt/emumaster/qml/img
	qmlimg.files = \
		../../../qml/img/input-accel.png \
		../../../qml/img/input-keyb.png \
		../../../qml/img/input-sixaxis.png \
		../../../qml/img/input-touch.png

	INSTALLS += qml qmlimg pad
}

contains(MEEGO_EDITION,harmattan) {
	CONFIG += qmsystem2
}
