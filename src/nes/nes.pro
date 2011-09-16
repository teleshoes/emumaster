include(../machine.pri)
LIBS += -lcrc32
QT += opengl declarative

HEADERS += \
    gamegeniecodelistmodel.h \
    gamegeniecode.h \
    mapper/mapper000.h \
    mapper/mapper255.h \
    mapper/mapper254.h \
    mapper/mapper252.h \
    mapper/mapper251.h \
    mapper/mapper246.h \
    mapper/mapper244.h \
    mapper/mapper243.h \
    mapper/mapper242.h \
    mapper/mapper241.h \
    mapper/mapper240.h \
    mapper/mapper236.h \
    mapper/mapper235.h \
    mapper/mapper233.h \
    mapper/mapper232.h \
    mapper/mapper231.h \
    mapper/mapper230.h \
    mapper/mapper229.h \
    mapper/mapper228.h \
    mapper/mapper227.h \
    mapper/mapper226.h \
    mapper/mapper225.h \
    mapper/mapper222.h \
    mapper/mapper202.h \
    mapper/mapper201.h \
    mapper/mapper200.h \
    mapper/mapper071.h \
    mapper/mapper070.h \
    mapper/mapper069.h \
    mapper/mapper068.h \
    mapper/mapper067.h \
    mapper/mapper066.h \
    mapper/mapper065.h \
    mapper/mapper064.h \
    mapper/mapper062.h \
    mapper/mapper061.h \
    mapper/mapper060.h \
    mapper/mapper058.h \
    mapper/mapper057.h \
    mapper/mapper051.h \
    mapper/mapper050.h \
    mapper/mapper048.h \
    mapper/mapper047.h \
    mapper/mapper046.h \
    mapper/mapper045.h \
    mapper/mapper044.h \
    mapper/mapper043.h \
    mapper/mapper042.h \
    mapper/mapper041.h \
    mapper/mapper040.h \
    mapper/mapper034.h \
    mapper/mapper033.h \
    mapper/mapper032.h \
    mapper/mapper027.h \
    mapper/mapper026.h \
    mapper/mapper025.h \
    mapper/mapper024.h \
    mapper/mapper023.h \
    mapper/mapper022.h \
    mapper/mapper021.h \
    mapper/mapper019.h \
    mapper/mapper018.h \
    mapper/mapper017.h \
    mapper/mapper016.h \
    mapper/mapper015.h \
    mapper/mapper013.h \
    mapper/mapper012.h \
    mapper/mapper011.h \
    mapper/mapper010.h \
    mapper/mapper009.h \
    mapper/mapper008.h \
    mapper/mapper007.h \
    mapper/mapper006.h \
    mapper/mapper005.h \
    mapper/mapper004.h \
    mapper/mapper003.h \
    mapper/mapper002.h \
    mapper/mapper001.h \
    apu.h \
    apuchannel.h \
    apudmchannel.h \
    apunoisechannel.h \
    apurectanglechannel.h \
    aputrianglechannel.h \
    cpu.h \
    disk.h \
    pad.h \
    machine.h \
    mapper.h \
    ppu.h \
    eeprom.h

SOURCES += \
    gamegeniecodelistmodel.cpp \
    gamegeniecode.cpp \
    mapper/mapper000.cpp \
    mapper/mapper007.cpp \
    mapper/mapper006.cpp \
    mapper/mapper005.cpp \
    mapper/mapper004.cpp \
    mapper/mapper003.cpp \
    mapper/mapper002.cpp \
    mapper/mapper001.cpp \
    ppuregisters.cpp \
    ppupalette.cpp \
    ppu.cpp \
    pad.cpp \
    mapper.cpp \
    machine.cpp \
    disk.cpp \
    cpu.cpp \
    apu.cpp \
    apuchannel.cpp \
    apudmchannel.cpp \
    apunoisechannel.cpp \
    apurectanglechannel.cpp \
    aputrianglechannel.cpp \
    cpu_tables.cpp \
    eeprom.cpp

unix {
	qml.path = /opt/emumaster/qml/nes
	qml.files = \
		../../qml/nes/main.qml \
		../../qml/nes/MainPage.qml \
		../../qml/nes/SettingsPage.qml \
		../../qml/nes/CheatPage.qml

	gameclassify.path = /usr/share/policy/etc/syspart.conf.d
	gameclassify.files += $${TARGET}.conf

	INSTALLS += qml gameclassify
}
#    mapper/mapper255.cpp \
#    mapper/mapper254.cpp \
#    mapper/mapper252.cpp \
#    mapper/mapper251.cpp \
#    mapper/mapper246.cpp \
#    mapper/mapper244.cpp \
#    mapper/mapper243.cpp \
#    mapper/mapper242.cpp \
#    mapper/mapper241.cpp \
#    mapper/mapper240.cpp \
#    mapper/mapper236.cpp \
#    mapper/mapper235.cpp \
#    mapper/mapper233.cpp \
#    mapper/mapper232.cpp \
#    mapper/mapper231.cpp \
#    mapper/mapper230.cpp \
#    mapper/mapper229.cpp \
#    mapper/mapper228.cpp \
#    mapper/mapper227.cpp \
#    mapper/mapper226.cpp \
#    mapper/mapper225.cpp \
#    mapper/mapper222.cpp \
#    mapper/mapper202.cpp \
#    mapper/mapper201.cpp \
#    mapper/mapper200.cpp \
#    mapper/mapper071.cpp \
#    mapper/mapper070.cpp \
#    mapper/mapper069.cpp \
#    mapper/mapper068.cpp \
#    mapper/mapper067.cpp \
#    mapper/mapper066.cpp \
#    mapper/mapper065.cpp \
#    mapper/mapper064.cpp \
#    mapper/mapper062.cpp \
#    mapper/mapper061.cpp \
#    mapper/mapper060.cpp \
#    mapper/mapper058.cpp \
#    mapper/mapper057.cpp \
#    mapper/mapper051.cpp \
#    mapper/mapper050.cpp \
#    mapper/mapper048.cpp \
#    mapper/mapper047.cpp \
#    mapper/mapper046.cpp \
#    mapper/mapper045.cpp \
#    mapper/mapper044.cpp \
#    mapper/mapper043.cpp \
#    mapper/mapper042.cpp \
#    mapper/mapper041.cpp \
#    mapper/mapper040.cpp \
#    mapper/mapper034.cpp \
#    mapper/mapper033.cpp \
#    mapper/mapper032.cpp \
#    mapper/mapper027.cpp \
#    mapper/mapper026.cpp \
#    mapper/mapper025.cpp \
#    mapper/mapper024.cpp \
#    mapper/mapper023.cpp \
#    mapper/mapper022.cpp \
#    mapper/mapper021.cpp \
#    mapper/mapper019.cpp \
#    mapper/mapper018.cpp \
#    mapper/mapper017.cpp \
#    mapper/mapper016.cpp \
#    mapper/mapper015.cpp \
#    mapper/mapper013.cpp \
#    mapper/mapper012.cpp \
#    mapper/mapper011.cpp \
#    mapper/mapper010.cpp \
#    mapper/mapper009.cpp \
#    mapper/mapper008.cpp \












