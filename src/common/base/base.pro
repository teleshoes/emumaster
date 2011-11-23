include(../common.pri)
QT += declarative opengl network
LIBS += -lpulse
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
    stateimageprovider.h

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
    stateimageprovider.cpp

unix {
	qml.path = /opt/emumaster/qml/base
	qml.files = \
		../../../qml/base/main.qml \
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
		../../../data/pad-left.png \
		../../../data/pad-right-abxy.png \
		../../../data/pad-right-psx.png

	qmlimg.path = /opt/emumaster/qml/img
	qmlimg.files = ../../../qml/img/*
	INSTALLS += qml qmlimg pad
}








