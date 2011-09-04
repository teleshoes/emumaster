#ifndef ROMGALLERY_H
#define ROMGALLERY_H

class RomListModel;
#include <QDeclarativeView>

class RomGallery : public QDeclarativeView {
    Q_OBJECT
public:
    explicit RomGallery(QWidget *parent = 0);
	~RomGallery();

	Q_INVOKABLE void launch(const QString &diskName);
	Q_INVOKABLE bool addIconToHomeScreen(const QString &diskName, qreal scale, int x, int y);
	Q_INVOKABLE void donate();
	Q_INVOKABLE void homepage();
signals:
	void romUpdate();
private slots:
	void onProcessFinished();
	void emitRomUpdate();
private:
	QImage applyMaskAndOverlay(const QImage &icon);

	RomListModel *m_romListModel;
};

#endif // ROMGALLERY_H
