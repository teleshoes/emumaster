#include "romgallery.h"
#include "imachine.h"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	IMachine::buildLocalDirTree();

	RomGallery view;
#if defined(MEEGO_EDITION_HARMATTAN)
	view.showFullScreen();
#else
	view.resize(854, 480);
	view.show();
#endif
	return app.exec();
}
