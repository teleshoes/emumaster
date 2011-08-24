#ifndef MACHINEIMAGEPROVIDER_H
#define MACHINEIMAGEPROVIDER_H

class MachineView;
class MachineStateListModel;
#include <QDeclarativeImageProvider>

class MachineImageProvider : public QDeclarativeImageProvider {
public:
	explicit MachineImageProvider(MachineView *machineView, MachineStateListModel *stateListModel);
	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
private:
	MachineView *m_machineView;
	MachineStateListModel *m_stateListModel;
};

#endif // MACHINEIMAGEPROVIDER_H
