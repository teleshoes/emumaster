#include "thread.h"
#include "imachine.h"

static Thread *t = 0;

extern "C" void synchronize_machine() {
	t->emitFrameGen();
}

Thread::Thread(IMachine *m, QObject *parent) :
	QThread(parent), m(m)
{
	t = this;
}

void Thread::run() {
	m->setDisk("/home/user/MyDocs/emumaster/gba/Sonic Advance");
}

void Thread::emitFrameGen() {
	emit frameGen();
}
