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

	int N = 5000;
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

	printf("Start Collecting Data!\n");
	FILE *fptr;
	fptr = fopen("dataMagRaw.csv", "w");
	if (fptr != NULL) {
		int i = 0;
		while (!done && i < N) {
				//if (imuraw.isDataAccReady()) imuraw.getDataAccRaw(accdata);
				//if (imuraw.isDataGyroReady()) imuraw.getDataGyroRaw(gyrdata);
				while (!imuraw.isDataMagReady()) { delay(100); }
				imuraw.getDataMagRaw(magdata);

				fprintf(fptr, "%d,%d,%d\r\n", magdata[0], magdata[1], magdata[2]);

				delay(10000);
				i++;
		}
		fclose(fptr); //fptr is the file pointer associated with file to be closed.
		printf("Finish Collecting Data!\n");
		printf("Data stored in dataMagRaw.csv\n");
	} else {
		printf("Error open file!");
		exit(1);
	}

	return 0;
}

void register_sig_handler() {
	signal(SIGINT, sigint_handler);
}

void sigint_handler(int sig) {
	done = 1;
}
