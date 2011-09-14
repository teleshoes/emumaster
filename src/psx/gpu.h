#ifndef PSXGPU_H
#define PSXGPU_H

#include <imachine.h>

class PsxGpu : public QObject {
	Q_OBJECT
public:
	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

extern PsxGpu psxGpu;

#endif // PSXGPU_H
