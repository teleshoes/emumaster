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

OTHER_FILES += \
    ../../qml/gallery/main.qml \
    ../../qml/gallery/RomChoosePage.qml \
    ../../qml/gallery/GalleryPage.qml \
    ../../qml/gallery/ListPage.qml \
	../../qml/common/MySectionScroller.js \
	../../qml/common/MySectionScroller.qml \
	../../qml/common/constants.js \
	../../qml/common/MyListDelegate.qml \
	../../qml/common/SectionScrollerLabel.qml \
	../../qml/common/CoverFlow.qml \
	../../qml/common/CoverFlowDelegate.qml







