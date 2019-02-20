#include <stdint.h>

#ifndef IIMURAW_H
#define IIMURAW_H

class IImuRaw // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
	uint8_t ID = 0; // Overwrite ID if you want more devices
	uint8_t ACCID = 0;
	uint8_t GYROID = 1;
	uint8_t MAGID = 2;
	virtual bool getDataImuRaw(int16_t (&data)[9]) = 0;
	virtual bool getDataAccRaw(int16_t (&data)[3]) = 0;
	virtual bool getDataGyroRaw(int16_t (&data)[3]) = 0;
	virtual bool getDataMagRaw(int16_t (&data)[3]) = 0;
	virtual bool isDataReady() = 0;
	virtual bool isDataAccReady() = 0;
	virtual bool isDataGyroReady() = 0;
	virtual bool isDataMagReady() = 0;
};

#endif
