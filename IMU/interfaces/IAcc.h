#include <stdint.h>

#ifndef IACC_H
#define IACC_H

class IAcc // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
	virtual bool getDataAccRaw(int16_t (&data)[3]) = 0;
	virtual bool isDataAccReady() = 0;
};

#endif
