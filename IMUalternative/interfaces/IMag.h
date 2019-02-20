#include <stdint.h>

#ifndef IMAG_H
#define IMAG_H

class IMag { // @suppress("Class has a virtual method and non-virtual destructor")
  public:
	virtual bool getDataMagRaw(int16_t (&data)[3]) = 0;
  virtual bool isDataMagReady() = 0;
};

#endif
