#ifndef ROMGALLERY_H
#define ROMGALLERY_H

class RomListModel;
class MachineView;
#include <QDeclarativeView>

class RomGallery : public QDeclarativeView {
    Q_OBJECT
public:
    explicit RomGallery(QWidget *parent = 0);
	~RomGallery();

	Q_INVOKABLE bool launch(const QString &machineName, const QString &diskName);
protected:
	void closeEvent(QCloseEvent *e);
private slots:
	void onMachineViewDestroyed();
private:
	RomListModel *m_romListModel;
	QString m_diskName;
	MachineView *m_machineView;
	bool m_wantClose;
};

#endif // ROMGALLERY_H
