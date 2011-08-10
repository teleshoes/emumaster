#ifndef NESMACHINEVIEW_H
#define NESMACHINEVIEW_H

class NesMachine;
#include "nes_global.h"
#include <QGLWidget>

class NES_EXPORT NesMachineView : public QGLWidget {
    Q_OBJECT
public:
	explicit NesMachineView(const QString &diskName, QWidget *parent = 0);
	NesMachine *machine() const;
signals:
	void showSettingsView();
protected:
	void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);
private:
	NesMachine *m_machine;
	QString m_error;
};

inline NesMachine *NesMachineView::machine() const
{ return m_machine; }

#endif // NESMACHINEVIEW_H
