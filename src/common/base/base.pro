include(../common.pri)
QT += declarative opengl
LIBS += -lpulse-simple

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
		../../../qml/base/MySectionScroller.js \
		../../../qml/base/MySectionScroller.qml \
		../../../qml/base/constants.js \
		../../../qml/base/MyListDelegate.qml \
		../../../qml/base/SectionScrollerLabel.qml \
		../../../qml/base/CoverFlow.qml \
		../../../qml/base/CoverFlowDelegate.qml \
		../../../qml/base/utils.js

	pad.path = /opt/emumaster/data
	pad.files = \
		../../../data/pad_arrows.png \
		../../../data/pad_buttons.png \
		../../../data/pad_select.png \
		../../../data/pad_start.png \
		../../../data/pause.png

	qmlimg.path = /opt/emumaster/qml/img
	qmlimg.files = ../../../qml/img/*
	INSTALLS += qml qmlimg pad
}




