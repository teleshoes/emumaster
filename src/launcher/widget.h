#ifndef WIDGET_H
#define WIDGET_H

#include "imachine.h"
#include <QGLWidget>

class Widget : public QGLWidget {
    Q_OBJECT
public:
	explicit Widget(QWidget *parent = 0);

	void setMachine(IMachine *m);

signals:

public slots:
protected:
	void paintEvent(QPaintEvent *);
private:
	IMachine *m;
};

#endif // WIDGET_H
