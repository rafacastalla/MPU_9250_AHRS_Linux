/**
 The MIT License (MIT)

 Copyright (c) 2018 Rafael Carbonell

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.


 @file       MPU9250.hpp
 @author     Rafael Carbonell
 @date       2018
 @version    0.1.0
 @copyright  The MIT License
 @brief      MPU9250 9-axis electronic compass I2C and SPI library

 */
#include "MPU9250.hpp"
#include <stdint.h>
#include <iostream>

#include "ICom.h"

MPU9250::MPU9250(uint8_t address) :
		_address(address) {
}
MPU9250::~MPU9250(){}

void MPU9250::attachInterface(ICom *comm) {
	com = comm;
}

void MPU9250::attachDelay(void (*delay)(uint32_t us)) {
	_delay = delay;
}

bool MPU9250::initialize() {
	bool ret = true;
	// Reset MPU all registers
	ret &= reset();
	if(ret == false) return false;

	// Set Clock frequency
	reg =  DefConfig::CLKSEL;
	ret &= _writeByte(Register::PWR_MGMT_1, &reg);
	_delay(200);
	if(ret == false) return false;

	// FCHOICE ACCEL AND GYRO
	reg = DefConfig::ACCEL_FCHOICE_B;
	ret &= _writeByte(Register::ACCEL_CONFIG2, &reg);
	if(ret == false) return false;

	reg = DefConfig::GYRO_FCHOICE_B;
	ret &= _writeByte(Register::GYRO_CONFIG, &reg);
	if(ret == false) return false;

	// BYPASS To use magnetometer like other slave in i2c - INT_PIN_CFG
	reg = DefConfig::BYPASS_EN;
	ret &= _writeByte(Register::INT_PIN_CFG, &reg);
	if(ret == false) return false;

	// Set default selection accel and gyro
	if (setGyroRange(_grange) == false) {
		// printf("Error setting default config - gyroscope configuration selection");
		return false;
	}

	if (setAccelRange(_arange) == false) {
		// printf("Error setting default config - accelerometer configuration selection");
		return false;
	}

	// Reset the offset registers to 0 - accel and gyro

	return true;
}

uint8_t MPU9250::getWhoAmI() {
	_readByte(Register::WHO_A_MI, &_device);
	return _device;
}

bool MPU9250::setGyroRange(MPU9250::GyroRange range) {
	uint8_t data;
	if (_readByte(Register::GYRO_CONFIG, &data)) {
		data = data | range << 3;
		if (_writeByte(Register::GYRO_CONFIG, &data)) {
			_grange = range;
			_gRes = getBaseGyroRange(range);
			return true;
		}
	}
	return false;
}

MPU9250::GyroRange MPU9250::getGyroRange() {
	return _grange;
}

bool MPU9250::setAccelRange(MPU9250::AccelRange range) {
	uint8_t data;
	if (_readByte(Register::ACCEL_CONFIG, &data)) {
		data = data | range << 3;
		if (_writeByte(Register::ACCEL_CONFIG, &data)) {
			_arange = range;
			_aRes = getBaseAccelRange(range);
			return true;
		}
	}
	return false;
}

MPU9250::AccelRange MPU9250::getAccelRange() {
	return _arange;
}

bool MPU9250::reset() {
	reg = DefConfig::H_RESET;
	bool ret = _writeByte(Register::PWR_MGMT_1, &reg);
	_delay(100);
	return ret;
}

bool MPU9250::readRawGyro(int16_t &x, int16_t &y, int16_t &z) {
	uint8_t rawData[6];  // x/y/z gyro register data stored here
	if (_readBytes(Register::GYRO_XOUT_H, rawData, (uint8_t) 6)) {
		x = ((int16_t) rawData[0] << 8) | rawData[1];
		y = ((int16_t) rawData[2] << 8) | rawData[3];
		z = ((int16_t) rawData[4] << 8) | rawData[5];
		return true;
	}
	return false;
}

bool MPU9250::readRawGyro(int16_t (&gyr)[3]) {
	return readRawGyro(gyr[0], gyr[1], gyr[2]);
}

bool MPU9250::readGyro(float &x, float &y, float &z) {
	int16_t rawData[3];
	if (readRawGyro(rawData[X], rawData[Y], rawData[Z])) {
		x = ((float) rawData[X]) * _gRes;
		y = ((float) rawData[Y]) * _gRes;
		z = ((float) rawData[Z]) * _gRes;
		return true;
	}
	return false;
}

bool MPU9250::readGyro(float (&gyr)[3]) {
	return readGyro(gyr[0], gyr[1], gyr[2]);
}

bool MPU9250::readRawAccel(int16_t &x, int16_t &y, int16_t &z) {
	uint8_t rawData[6];  // x/y/z acc register data stored here
	if (_readBytes(Register::ACCEL_XOUT_H, rawData, (uint8_t) 6)) {
		x = ((int16_t) rawData[0] << 8) | rawData[1];
		y = ((int16_t) rawData[2] << 8) | rawData[3];
		z = ((int16_t) rawData[4] << 8) | rawData[5];
		return true;
	}
	return false;
}

bool MPU9250::readRawAccel(int16_t (&acc)[3]) {
	return readRawAccel(acc[X], acc[Y], acc[Z]);
}

bool MPU9250::readAccel(float &x, float &y, float &z) {
	int16_t rawData[3];
	if (readRawAccel(rawData[X], rawData[Y], rawData[Z])) {
		x = ((float) rawData[X]) * _aRes;
		y = ((float) rawData[Y]) * _aRes;
		z = ((float) rawData[Z]) * _aRes;
		return true;
	}
	return false;
}
bool MPU9250::readAccel(float (&acc)[3]) {
	return readAccel(acc[X], acc[Y], acc[Z]);
}

float MPU9250::getBaseGyroRange(MPU9250::GyroRange grange){
	  switch (grange)
	  {
	  // Possible gyro scales (and their register bit settings) are:
	  // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
	    case MPU9250::GyroRange::GYRO_RANGE_250DPS:
	          _gRes = 250.0/base;
	          return _gRes;
	          break;
	    case MPU9250::GyroRange::GYRO_RANGE_500DPS:
	          _gRes = 500.0/base;
	          return _gRes;
	          break;
	    case MPU9250::GyroRange::GYRO_RANGE_1000DPS:
	         _gRes = 1000.0/base;
	         return _gRes;
	         break;
	    case MPU9250::GyroRange::GYRO_RANGE_2000DPS:
	          _gRes = 2000.0/base;
	         return _gRes;
	         break;
	  }
	  return 0;
}

float MPU9250::getBaseAccelRange(MPU9250::AccelRange arange){
	  switch (arange)
	  {
	  // Possible accelerometer scales (and their register bit settings) are:
	  // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
	        // Here's a bit of an algorith to calculate DPS/(ADC tick) based on that 2-bit value:
	    case MPU9250::AccelRange::ACCEL_RANGE_2G:
	         _aRes = 2.0f/base;
	         return _aRes;
	         break;
	    case MPU9250::AccelRange::ACCEL_RANGE_4G:
	         _aRes = 4.0f/base;
	         return _aRes;
	         break;
	    case MPU9250::AccelRange::ACCEL_RANGE_8G:
	         _aRes = 8.0f/base;
	         return _aRes;
	         break;
	    case MPU9250::AccelRange::ACCEL_RANGE_16G:
	         _aRes = 16.0f/base;
	         return _aRes;
	         break;
	  }
	return 0;
}

bool MPU9250::getDataGyroRaw(int16_t (&data)[3]){
	return readRawGyro(data[X], data[Y], data[Z]);
}

bool MPU9250::getDataAccRaw(int16_t (&data)[3]){
	return readRawAccel(data[X], data[Y], data[Z]);
}

bool MPU9250::isDataGyroReady(){ return true; }
bool MPU9250::isDataAccReady(){ return true; }


bool MPU9250::_writeByte(Register reg, uint8_t* data) {
	return _writeBytes(reg, data);
}

bool MPU9250::_readByte(Register reg, uint8_t* data) {
	return _readBytes(reg, data);
}

bool MPU9250::_readBytes(Register reg, uint8_t* data, uint8_t size) {
	if (com == nullptr)
		return false;
	return com->readCOM(_address, (uint8_t) reg, data, size);
}

bool MPU9250::_writeBytes(Register reg, uint8_t* data, uint8_t size) {
	if (com == nullptr)
		return false;
	return com->writeCOM(_address, (uint8_t) reg, data, size);
}
