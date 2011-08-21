#ifndef MACHINEIMAGEPROVIDER_H
#define MACHINEIMAGEPROVIDER_H

class MachineView;
#include <QDeclarativeImageProvider>

class MachineImageProvider : public QDeclarativeImageProvider {
public:
	explicit MachineImageProvider(MachineView *machineView);
	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
private:
	MachineView *m_machineView;
};

#endif // MACHINEIMAGEPROVIDER_H
