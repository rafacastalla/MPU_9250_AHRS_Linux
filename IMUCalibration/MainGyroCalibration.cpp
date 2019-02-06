#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <string>       // std::string
#include <sstream>      // std::stringstream, std::stringbuf
#include <stdio.h>
#include <assert.h>     /* assert */

#include <signal.h>

#include "MPU9250.hpp"
#include "AK8963.hpp"
#include "I2C.hpp"
#include "timeUtils.h"
#include "ImuRaw.hpp"

int done = 0;
void register_sig_handler();
void sigint_handler(int sig);

int main(int argc, char **argv) {

	int N = 2000;
	// Auxiliar data struct
	bool ret = true;

//////////////////////////////////////////////////////////////////

// Initialization comunication bus
	int i2cId = 1;
	I2C i2c(i2cId);

//////////////////////////////////////////////////////////////////

	uint8_t imuAddress = 0x68;
	MPU9250 mpu(imuAddress);

	// The callback functions have to be attached to the object.
	mpu.attachInterface(&i2c);

	// The AK8963 also needs a delay function (in microseconds).
	mpu.attachDelay(delay);

	// Select Configuration
	mpu.setAccelRange(MPU9250::ACCEL_RANGE_16G);
	mpu.setGyroRange(MPU9250::GYRO_RANGE_2000DPS);

	// Initialize the mpu and power on the
	ret &= mpu.initialize();
	assert(ret != false);

//////////////////////////////////////////////////////////////////

	uint8_t magAddress = 0x0C;
	AK8963 mag(magAddress);

	// The callback functions have to be attached to the object.
	mag.attachInterface(&i2c);

	// The AK8963 also needs a delay function (in microseconds).
	mag.attachDelay(delay);

	// Initialize the mpu and power on the
	ret &= mag.checkConnection();
	assert(ret!=false);

	ret &= mag.initialize();
	assert(ret != false);

	// Select Configuration
	mag.resolution(AK8963::BITS_16);
	mag.setMode(AK8963::CONTINUOUS_MEASUREMENT_100HZ);

//////////////////////////////////////////////////////////////////

// Generic intergace for IMU 9 DOF
	ImuRaw imuraw;
	imuraw.attachInterface(&mpu, &mpu, &mag);
	int16_t accdata[3], gyrdata[3], magdata[3];


	//Register SIGINT handler for managed program termination
	register_sig_handler();

	printf("Start Collecting Data\n");
	printf("Don't move\n");
	printf("Calculating noise average of Gyroscope\n");

	int i = 0;
	float avggyr[3] = {0, 0, 0};
	while (!done || i < N) {
			if (imuraw.isDataAccReady()) imuraw.getDataAccRaw(accdata);
			//if (imuraw.isDataGyroReady()) imuraw.getDataGyroRaw(gyrdata);
			//if (imuraw.isDataMagReady()) imuraw.getDataMagRaw(magdata);

			avggyr[0] += gyrdata[0];
			avggyr[1] += gyrdata[1];
			avggyr[2] += gyrdata[2];
			i++;
	}
	printf("Finish Collecting Data!\n");
	printf("The average of Gyroscope is: \n");
	printf("X: %.3f\n", avggyr[0]/((float)N) );
	printf("Y: %.3f\n", avggyr[1]/((float)N) );
	printf("Z: %.3f\n", avggyr[2]/((float)N) );

	return 0;
}

void register_sig_handler() {
	signal(SIGINT, sigint_handler);
}

void sigint_handler(int sig) {
	done = 1;
}
