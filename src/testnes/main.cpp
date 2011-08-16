#include "nesmachineview.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	NesMachineView view("../data/nes/1200-in-1 [p1].nes");
	view.show();
	return app.exec();
}
