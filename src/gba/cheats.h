#ifndef GBACHEATS_H
#define GBACHEATS_H

#include "common.h"

class GbaGameSharkCheat {
public:
	bool parse(const QString &cheat, bool v3, u16 deadface);
private:
	void changeEncryption(u16 data, bool v3);
	void decrypt(u32 *address, u32 *data);

	u32 m_seeds[4];
};

void process_cheats();

#endif // GBACHEATS_H
