#include "romgallery.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	QDir::setCurrent(app.applicationDirPath());
	RomGallery view;
#if defined(MEEGO_EDITION_HARMATTAN)
	view.showFullScreen();
#else
	view.resize(854, 480);
	view.show();
#endif
	return app.exec();
}
