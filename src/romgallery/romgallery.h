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

	Q_INVOKABLE bool launch(const QString &diskName);
	Q_INVOKABLE bool addIconToHomeScreen(const QString &diskName, qreal scale, int x, int y);
protected:
	void closeEvent(QCloseEvent *e);
private slots:
	void onMachineViewDestroyed();
private:
	QImage applyMaskAndOverlay(const QImage &icon);

	RomListModel *m_romListModel;
	QString m_diskName;
	MachineView *m_machineView;
};

#endif // ROMGALLERY_H
