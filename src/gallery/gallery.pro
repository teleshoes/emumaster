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
    ../../qml/gallery/MySectionScroller.js \
    ../../qml/gallery/MySectionScroller.qml \
    ../../qml/gallery/constants.js \
    ../../qml/gallery/MyListDelegate.qml \
    ../../qml/gallery/SectionScrollerLabel.qml \
    ../../qml/gallery/CoverFlow.qml \
    ../../qml/gallery/CoverFlowDelegate.qml







