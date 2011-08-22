#include "romgallery.h"
#include "romimageprovider.h"
#include "romlistmodel.h"
#include "imachine.h"
#include "machineview.h"
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QTimer>
#include <QCloseEvent>

RomGallery::RomGallery(QWidget *parent) :
	QDeclarativeView(parent),
	m_machineView(0),
	m_wantClose(false) {
	m_romListModel = new RomListModel(this);
	engine()->addImageProvider("rom", new RomImageProvider());
	rootContext()->setContextProperty("romListModel", m_romListModel);
	rootContext()->setContextProperty("romGallery", this);
	setSource(QUrl::fromLocalFile("../qml/gallery/main.qml"));
}

RomGallery::~RomGallery() {
}

bool RomGallery::launch(const QString &machineName, const QString &diskName) {
	IMachine *machine = IMachine::loadMachine(machineName);
	if (!machine)
		return false;
	m_diskName = diskName;
	m_machineView = new MachineView(machine, diskName, this);
	QObject::connect(m_machineView, SIGNAL(destroyed()), SLOT(onMachineViewDestroyed()));
	m_machineView->setAttribute(Qt::WA_DeleteOnClose);
	return true;
}

void RomGallery::onMachineViewDestroyed() {
	if (m_wantClose) {
		close();
	} else {
		// TODO update image
	}
}

void RomGallery::closeEvent(QCloseEvent *e) {
	m_wantClose = true;
	if (m_machineView && m_machineView->isRunning()) {
		m_machineView->close();
		e->ignore();
	} else {
		e->accept();
	}
}
