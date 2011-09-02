#ifndef THREAD_H
#define THREAD_H

class IMachine;
#include <QThread>

class Thread : public QThread {
    Q_OBJECT
public:
	explicit Thread(IMachine *m, QObject *parent = 0);
	void emitFrameGen();
signals:
	void frameGen();
public slots:
protected:
	void run();

	IMachine *m;
};

#endif // THREAD_H
