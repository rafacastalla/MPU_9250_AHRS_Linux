#include <stdint.h>

#ifndef IGYRO_H
#define IGYRO_H

class IGyro{ // @suppress("Class has a virtual method and non-virtual destructor")
  public:
	virtual bool getDataGyroRaw(int16_t (&data)[3]) = 0;
  virtual bool isDataGyroReady() = 0;
};

#endif
