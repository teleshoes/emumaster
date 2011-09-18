#ifndef PSXSPUNULL_H
#define PSXSPUNULL_H

#include "spu.h"

class PsxSpuNull : public PsxSpu {
public:
	bool init();

	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

extern PsxSpuNull psxSpuNull;

#endif // PSXSPUNULL_H
