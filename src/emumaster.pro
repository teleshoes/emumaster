TEMPLATE = subdirs

SUBDIRS += \
	common \
	gba \
	nes \
	psx \
	snes \
	romgallery

OTHER_FILES += \
	../todo.txt \
	../include/numeric.h \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    ../wiki.txt


