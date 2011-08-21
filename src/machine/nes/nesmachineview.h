#ifndef NESMACHINEVIEW_H
#define NESMACHINEVIEW_H

#include <machineview.h>

class NesMachineView : public MachineView {
    Q_OBJECT
public:
	explicit NesMachineView(const QString &path, QWidget *parent = 0);
	~NesMachineView();
};

#endif // NESMACHINEVIEW_H
