include(../common.pri)
QT += declarative opengl network
LIBS += -lpulse

DEFINES += BASE_PROJECT

HEADERS += \
    machineview.h \
    imachine.h \
    machinethread.h \
    hostaudio.h \
    hostvideo.h \
    hostinput.h \
    machineimageprovider.h \
    machinestatelistmodel.h \
    settingsview.h \
    base_global.h

SOURCES += \
    machineview.cpp \
    imachine.cpp \
    machinethread.cpp \
    hostaudio.cpp \
    hostvideo.cpp \
    hostinput.cpp \
    machineimageprovider.cpp \
    machinestatelistmodel.cpp \
    settingsview.cpp

unix {
	qml.path = /opt/emumaster/qml/base
	qml.files = \
		../../../qml/base/CoverFlow.qml \
		../../../qml/base/CoverFlowDelegate.qml \
		../../../qml/base/DefaultSettings.qml \
		../../../qml/base/EMButtonOption.qml \
		../../../qml/base/EMSwitchOption.qml \
		../../../qml/base/MySectionScroller.js \
		../../../qml/base/MySectionScroller.qml \
		../../../qml/base/MyListDelegate.qml \
		../../../qml/base/SectionScrollerLabel.qml \
		../../../qml/base/StatePage.qml \
		../../../qml/base/constants.js \
		../../../qml/base/utils.js

	pad.path = /opt/emumaster/data
	pad.files = \
		../../../data/pad_arrows.png \
		../../../data/pad_buttons.png \
		../../../data/pad_select.png \
		../../../data/pad_start.png \
		../../../data/pause.png \
		../../../data/quit.png

	qmlimg.path = /opt/emumaster/qml/img
	qmlimg.files = ../../../qml/img/*
	INSTALLS += qml qmlimg pad
}




