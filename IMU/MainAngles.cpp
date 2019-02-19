#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <string>       // std::string
#include <sstream>      // std::stringstream, std::stringbuf
#include <stdio.h>
#include <assert.h>     /* assert */
#include <math.h>       /* atan2 */

#include <signal.h>

#include "MPU9250.hpp"
#include "AK8963.hpp"
#include "I2C.hpp"
#include "timeUtils.h"
#include "ImuRaw.hpp"

//#include "Eigen"
//using namespace Eigen;

int done = 0;
void register_sig_handler();
void sigint_handler(int sig);

#define X 0
#define Y 1
#define Z 2

#define PI 3.14159265
#define RAD2DEG 180.0 / PI
#define DEG2RAD PI / 180.0

int main(int argc, char **argv) {

	int N = 10000;
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

	float gyroRes = MPU9250::GYRO_RANGE_2000DPS;
	float accRes = MPU9250::ACCEL_RANGE_16G;

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

	float accCalData[3], gyrCalData[3], magCalData[3];

	float accResX = accRes / 2730.6;
	float accResY = accRes / 3229.9;
	float accResZ = accRes / 2469.1;

	float angles[N][3];
	long times[N][4];

	float cp; // Cos Pitch
	float sp; // Sin Pitch
	float cr; // Cos Roll
	float sr; // Sin Roll

	float mcx, mcy; // MagX Compensated, MagY Compensated

	//Register SIGINT handler for managed program termination
	register_sig_handler();

	printf("Start Collecting Data!\n");
	FILE *fptr;
	fptr = fopen("angles.csv", "w");
	if (fptr != NULL) {
		int i = 0;

	    struct timespec ts;
	    clock_gettime(CLOCK_MONOTONIC, &ts);

		while (!done && i < N) {

				times[i][0] = getCurrentMicroseconds();

				while (!imuraw.isDataMagReady()) { delay(100); }
				imuraw.getDataMagRaw(magdata);
				if (imuraw.isDataAccReady()) imuraw.getDataAccRaw(accdata);
				if (imuraw.isDataGyroReady()) imuraw.getDataGyroRaw(gyrdata);

				times[i][1] = getCurrentMicroseconds();

				gyrCalData[X] = (((float)gyrdata[X]) - (12.542)) * gyroRes;
				gyrCalData[Y] = (((float)gyrdata[Y]) - (-19.81)) * gyroRes;
				gyrCalData[Z] = (((float)gyrdata[Z]) - (-6.555)) * gyroRes;

				accCalData[X] = (((float)gyrdata[X]) - (-192.4)) * accResX;
				accCalData[Y] = (((float)gyrdata[Y]) - (-94.4)) * accResY;
				accCalData[Z] = (((float)gyrdata[Z]) - (-1043.5)) * accResZ;

				magCalData[X] = (((float)gyrdata[X]) - (-41.81));
				magCalData[Y] = (((float)gyrdata[Y]) - (96.24));
				magCalData[Z] = (((float)gyrdata[Z]) - (-125.2));

				magCalData[X] = 0.981  * magCalData[X] + -0.003 * magCalData[Y] + -0.027 * magCalData[Z];
				magCalData[Y] = -0.003 * magCalData[X] + 0.981  * magCalData[Y] + 0.0032 * magCalData[Z];
				magCalData[Z] = -0.027 * magCalData[X] + 0.0032 * magCalData[Y] + 0.96   * magCalData[Z];

				times[i][2] = getCurrentMicroseconds();

				// PITCH
				angles[i][1] =  gyrCalData[X] * 0.01 * 0.98
						+ atan2f((float)accCalData[Y], (float)accCalData[Z]) * 0.02;

				// ROLL
				angles[i][0] =  gyrCalData[Y] * 0.01 * 0.98
						+ atan2f((float)accCalData[X], (float)accCalData[Z]) * 0.02;

				cp = cosf(angles[i][1] * DEG2RAD);
				sp = sinf(angles[i][1] * DEG2RAD);

				cr = cosf(angles[i][0] * DEG2RAD);
				sr = sinf(angles[i][0] * DEG2RAD);

				mcx = magCalData[X] * cr - magCalData[Z] * sr;
				mcy = magCalData[X] * sp * sr + magCalData[Y] * cp + magCalData[Z] * sp * cr;

				angles[i][2] = atan2f(mcy, mcx) * RAD2DEG;

				times[i][3] = getCurrentMicroseconds();

				i++;
				sleep_until(&ts, 10000);
		}

		N = i;
		for(i = 0; i < N; i++){
			fprintf(fptr, "%f,%f,%f", angles[i][0], angles[i][1], angles[i][2]);
			fprintf(fptr, ",%ld,%ld,%ld,%ld\r\n", times[i][0], times[i][1], times[i][2], times[i][3]);
		}

		fclose(fptr); //fptr is the file pointer associated with file to be closed.
		printf("Finish Collecting Data!\n");
		printf("Data stored in angles.csv\n");
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
