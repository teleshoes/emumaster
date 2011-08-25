#include "machineview.h"
#include "imachine.h"
#include <QApplication>
#include <QDir>
#include <QTextStream>
#include <stdio.h>

int main(int argc, char *argv[]) {
	if (argc != 3) {
		QTextStream(stderr) << "use: launcher machine-name disk-name";
		return -1;
	}
	QApplication app(argc, argv);
	QDir::setCurrent(app.applicationDirPath());

	QString machineName = argv[1];
	IMachine *machine = IMachine::loadMachine(machineName);
	if (!machine) {
		QTextStream(stderr) << qPrintable(QString("Could not load %1 machine").arg(machineName));
		return -2;
	}
	MachineView *view = new MachineView(machine, argv[2]);
	return app.exec();
}
