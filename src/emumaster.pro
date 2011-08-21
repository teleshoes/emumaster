TEMPLATE = subdirs

SUBDIRS += \
	algorithm \
    component \
	machine \
	testnes

OTHER_FILES += \
	../todo.txt \
	../include/numeric.h \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog
