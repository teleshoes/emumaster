#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include <qdeclarative.h>
#include "qmlapplicationviewer.h"
#include "debugger.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

	qmlRegisterType<NesDebugger>();
	qmlRegisterType<NesProfiler>();
	NesDebugger *dbg = new NesDebugger();

    QmlApplicationViewer viewer;
	viewer.rootContext()->setContextProperty("dbg", dbg);
	viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/nesdebugger/main.qml"));
	viewer.showExpanded();

    return app->exec();
}
