/*
	Free for non-commercial use.
	For commercial use, separate licencing terms must be obtained.
	(c) Copyright 2011, elemental
*/

#ifndef CART_H
#define CART_H

#include <imachine.h>

class PicoCart {
public:
	PicoCart();
	~PicoCart();
	bool open(const QString &fileName, QString *error);
	bool openMegaCDBios(const QString &fileName, QString *error);
	void close();
private:
	void decodeSmd();
	bool allocateMem();
	void detect();
};

extern PicoCart picoCart;

#endif // CART_H
