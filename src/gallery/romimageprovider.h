#ifndef ROMIMAGEPROVIDER_H
#define ROMIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>

class RomImageProvider : public QDeclarativeImageProvider {
public:
    RomImageProvider();
	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
private:
	QImage m_noScreenShot;
};

#endif // ROMIMAGEPROVIDER_H
