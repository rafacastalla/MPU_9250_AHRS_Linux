#include <stdint.h>

#ifndef IIMUCAL_H
#define IIMUCAL_H

class IImuCal // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
	uint8_t ID = 0; // Overwrite ID if you want more devices
	uint8_t ACCID = 0;
	uint8_t GYROID = 1;
	uint8_t MAGID = 2;

	virtual bool getDataImuCal(int16_t (&data)[9]) = 0;
	virtual bool getDataAccCal(int16_t (&data)[3]) = 0;
	virtual bool getDataGyroCal(int16_t (&data)[3]) = 0;
	virtual bool getDataMagCal(int16_t (&data)[3]) = 0;

	virtual bool setDataAccCal(int16_t (&offsets)[3], int16_t (&scale)[3]) = 0;
	virtual bool setDataGyroCal(int16_t (&offsets)[3]) = 0;
	virtual bool setDataMagCal(int16_t (&offsets)[3], int16_t (&scale)[3][3]) = 0;

	virtual bool isDataReady() = 0;
	virtual bool isDataAccReady() = 0;
	virtual bool isDataGyroReady() = 0;
	virtual bool isDataMagReady() = 0;
};

#endif
