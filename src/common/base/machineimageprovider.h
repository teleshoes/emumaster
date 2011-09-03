#ifndef MACHINEIMAGEPROVIDER_H
#define MACHINEIMAGEPROVIDER_H

class IMachine;
class MachineStateListModel;
#include <QDeclarativeImageProvider>

class MachineImageProvider : public QDeclarativeImageProvider {
public:
	explicit MachineImageProvider(IMachine *machine, MachineStateListModel *stateListModel);
	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
private:
	QImage screenShotGrayscaled() const;

	IMachine *m_machine;
	MachineStateListModel *m_stateListModel;
};

#endif // MACHINEIMAGEPROVIDER_H
