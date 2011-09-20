#ifndef SPU_FRAN_H
#define SPU_FRAN_H

#include "spu.h"

class PsxSpuFran : public PsxSpu {
public:
	bool init();
	void shutdown();

	int fillBuffer(char *stream, int size);

	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

extern PsxSpuFran psxSpuFran;

#endif // SPU_FRAN_H
