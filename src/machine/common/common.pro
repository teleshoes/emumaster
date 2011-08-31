TARGET = machine_common
include(../machine.pri)
QT += declarative opengl
LIBS += -lpulse-simple -lgamegeniecode

win32 {
	QT += multimedia
}

DEFINES += MACHINE_COMMON_PROJECT

HEADERS += \
    machineview.h \
    machine_common_global.h \
    imachine.h \
    machinethread.h \
    hostaudio.h \
    hostvideo.h \
    hostinput.h \
    machineimageprovider.h \
    machinestatelistmodel.h \
    gamegeniecodelistmodel.h \
    settingsview.h

SOURCES += \
    machineview.cpp \
    imachine.cpp \
    machinethread.cpp \
    hostaudio.cpp \
    hostvideo.cpp \
    hostinput.cpp \
    machineimageprovider.cpp \
    machinestatelistmodel.cpp \
    gamegeniecodelistmodel.cpp \
    settingsview.cpp

unix {
	qml.path = /opt/emumaster/qml/common
	qml.files = \
		../../../qml/common/MySectionScroller.js \
		../../../qml/common/MySectionScroller.qml \
		../../../qml/common/constants.js \
		../../../qml/common/MyListDelegate.qml \
		../../../qml/common/SectionScrollerLabel.qml \
		../../../qml/common/CoverFlow.qml \
		../../../qml/common/CoverFlowDelegate.qml \
		../../../qml/common/utils.js

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


