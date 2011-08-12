#include "nesmachineview.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	NesMachineView view("../data/nes/F-1_Race.nes");
	view.show();
	return app.exec();
}
