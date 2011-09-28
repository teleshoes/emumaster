#ifndef ROMGALLERY_H
#define ROMGALLERY_H

class RomListModel;
#include <QDeclarativeView>

class RomGallery : public QDeclarativeView {
    Q_OBJECT
public:
    explicit RomGallery(QWidget *parent = 0);
	~RomGallery();

	Q_INVOKABLE void launch(int index, bool autoload);

	Q_INVOKABLE bool addIconToHomeScreen(int index, qreal scale, int x, int y);
	Q_INVOKABLE void removeIconFromHomeScreen(int index);
	Q_INVOKABLE bool iconInHomeScreenExists(int index);

	Q_INVOKABLE void donate();
	Q_INVOKABLE void homepage();
signals:
	void romUpdate();
	void detachUsb();
private slots:
	void emitRomUpdate();
private:
	QImage applyMaskAndOverlay(const QImage &icon);
	void homeScreenIconPaths(QString *desktopFilePath, QString *iconFilePath, const QString &diskTitle);

	RomListModel *m_romListModel;
};

#endif // ROMGALLERY_H
