#include "nesmachineview.h"
#include "nesppu.h"
#include "nesmachine.h"
#include "nesdisk.h"

NesMachineView::NesMachineView(const QString &path, QWidget *parent) :
	MachineView(parent) {
	setSourceRect(QRectF(8.0f, 1.0f, NesPpu::VisibleScreenWidth, NesPpu::VisibleScreenHeight));
	setDestRect(QRectF(171.0f, 0.0f, NesPpu::VisibleScreenWidth*2, NesPpu::VisibleScreenHeight*2));

	NesMachine *machine = new NesMachine();
	NesDisk *disk = new NesDisk(path);
	if (!machine->setDisk(disk)) {
		if (!disk->isLoaded())
			showError(QString("Could not load ROM file\n%1").arg(path));
		else
			showError(QString("Mapper %1 is not supported").arg(disk->mapperType()));
		delete machine;
	} else {
		setMachineAndQmlSettings(machine, "../qml/nes/main.qml");
	}
}

NesMachineView::~NesMachineView() {
}
