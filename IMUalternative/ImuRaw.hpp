/*
 * imuRaw.hpp
 *
 *  Created on: 30 ene. 2019
 *      Author: racarla
 */

#include "IImuRaw.h"
#include "ICom.h"
#include "IAcc.h"
#include "IGyro.h"
#include "IMag.h"
#include <stdint.h>

class ImuRaw : public IImuRaw {
public:
	virtual ~ImuRaw();
	bool attachInterface(IAcc *iacc, IGyro *igyro, IMag *imag);
	bool getDataImuRaw(int16_t (&data)[9]);
	bool getDataAccRaw(int16_t (&data)[3]);
	bool getDataGyroRaw(int16_t (&data)[3]);
	bool getDataMagRaw(int16_t (&data)[3]);
	bool isDataReady();
	bool isDataAccReady();
	bool isDataGyroReady();
	bool isDataMagReady();
private:
	int16_t accdata[3], gyrdata[3], magdata[3];
	IAcc *acc = nullptr;
	IGyro *gyro = nullptr;
	IMag *mag = nullptr;
};
