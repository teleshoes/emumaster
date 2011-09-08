#ifndef EMULATOR_H
#define EMULATOR_H

#include <QGLWidget>
#include <QThread>

class Widget : public QGLWidget {
	Q_OBJECT
public:
protected:
	void paintEvent(QPaintEvent *);
};

class Thread : public QThread {
	Q_OBJECT
public:
	void run();
	void emitFrameGen();
signals:
	void frameGen();
};

#endif // EMULATOR_H
