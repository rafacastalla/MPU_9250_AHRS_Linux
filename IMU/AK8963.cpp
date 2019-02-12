/**
 The MIT License (MIT)

 Copyright (c) 2016 Vasil Kalchev

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


 @file       AK8963.cpp
 @author     Vasil Kalchev
 @updated    Rafael Carbonell
 @date       2018
 @version    0.1.0
 @copyright  The MIT License
 @brief      AK8963 3-axis electronic compass I2C and SPI library
 */
#include "AK8963.hpp"
#include <stdint.h>

AK8963::AK8963(uint8_t address) :
_address(address) {
}

AK8963::~AK8963(){}

void AK8963::attachInterface(ICom *comm) {
	com = comm;
}

void AK8963::attachDelay(void (*delay)(uint32_t us)) {
	_delay = delay;
}

bool AK8963::initialize() {
	bool ret = true;
	ret &= softReset();
	_initializeSensitivityAdjustment();
	return ret;
}

uint8_t AK8963::getDeviceId() {
	uint8_t deviceId = 0;
	_readBytes(Register::WIA, &deviceId);
	return deviceId;
}

bool AK8963::checkConnection() {
	return (getDeviceId() == ID);
}

uint8_t AK8963::information() {
	uint8_t info = 0;
	_readBytes(Register::INFO, &info);
	return info;
}

bool AK8963::getDataReady() {
	bool dataReady = false;
	_readBit(Register::ST1, &dataReady, Bit::DRDY);
	return dataReady;
}

bool AK8963::getDataOverrun() {
	bool dataOverrun = false;
	_readBit(Register::ST1, &dataOverrun, Bit::DOR);
	return dataOverrun;
}

bool AK8963::getOverflow() {
	return _overflow;
}

void AK8963::setMode(Mode mode) {
	powerDown();
	_delay(100);
	_writeBits(Register::CNTL1, (uint8_t) mode, Bit::MODE, Length::MODE);
}

AK8963::Mode AK8963::getMode() {
	uint8_t mode = 0;
	_readBits(Register::CNTL1, &mode, Bit::MODE, Length::MODE);
	return (Mode) mode;
}

void AK8963::powerDown() {
	_writeBits(Register::CNTL1, (uint8_t) POWER_DOWN, Bit::MODE, Length::MODE);
}

void AK8963::startMeasurement() {
	_writeBits(Register::CNTL1, (uint8_t) SINGLE_MEASUREMENT, Bit::MODE,
			Length::MODE);
}

void AK8963::resolution(Resolution resolution) {
	_writeBit(Register::CNTL1, (bool) resolution, Bit::BIT);
	if (resolution == Resolution::BITS_14) {
		_sensitivity_ut = 0.6f;
	} else if (resolution == Resolution::BITS_16) {
		_sensitivity_ut = 0.15f;
	}
}

AK8963::Resolution AK8963::getResolution() {
	bool resolution = 0;
	_readBit(Register::CNTL1, &resolution, Bit::BIT);
	return (Resolution) resolution;
}

bool AK8963::softReset() {
	return _writeBit(Register::CNTL2, true, Bit::SRST);
}

bool AK8963::selfTest() {
	bool pass = true;
	Mode mode = getMode();
	powerDown();
	_delay(100);
	_writeBit(Register::ASTC, true, Bit::SELF);
	_writeBits(Register::CNTL1, (uint8_t) SELF_TEST, Bit::MODE, Length::MODE);
	float x = 0;
	float y = 0;
	float z = 0;
	while (!readMag(x, y, z))
		;
	_writeBit(Register::ASTC, false, Bit::SELF);
	powerDown();
	uint16_t range = 0;
	Resolution resolution = getResolution();
	if (resolution == BITS_14) {
		range = 50;
		if (z < -800 || z > -200)
			pass = false;
	} else if (resolution == BITS_16) {
		range = 200;
		if (z < -3200 || z > -800)
			pass = false;
	} else {
		pass = false;
	}
	if (x < -range || x > range || y < -range || y > range)
		pass = false;
	setMode(Mode::CONTINUOUS_MEASUREMENT_100HZ);
	return pass;
}

void AK8963::disableI2c() {
	areg = 0b00011011;
	_writeBytes(Register::I2CDIS, &areg);
}

bool AK8963::readMag(float &x, float &y, float &z) {
	uint8_t data[8];
	if (_readBytes(Register::ST1, data, 8)) {
		if ((data[0] & (1 << (uint8_t) Bit::DRDY) == 1)) {  // Data is ready
			if ((data[7] & (1 << (uint8_t) Bit::HOFL)) == 0) {  // No overflow
				x = ((float) (((int16_t) data[2] << 8) | (data[1])))
						* _sensitivity_ut;
				y = ((float) (((int16_t) data[4] << 8) | (data[3])))
						* _sensitivity_ut;
				z = ((float) (((int16_t) data[6] << 8) | (data[5])))
						* _sensitivity_ut;
				_overflow = false;
				return true;
			}  // overflow (Eut > 4912uT)
			_overflow = true;
		}  // data not ready
	}  // couldn't read registers
	return false;
}

bool AK8963::readMag(float (&mag)[3]) {
	uint8_t data[8];
	if (_readBytes(Register::ST1, data, 8)) {
		if ((data[0] & (1 << (uint8_t) Bit::DRDY) == 1)) {  // Data is ready
			if ((data[7] & (1 << (uint8_t) Bit::HOFL)) == 0) {  // No overflow
				mag[0] = ((float) (((int16_t) data[2] << 8) | (data[1])))
						* _xSensAdj;
				mag[1] = ((float) (((int16_t) data[4] << 8) | (data[3])))
						* _ySensAdj;
				mag[2] = ((float) (((int16_t) data[6] << 8) | (data[5])))
						* _zSensAdj;
				_overflow = false;
				return true;
			}  // overflow (Eut > 4912uT)
			_overflow = true;
		}  // data not ready
	}  // couldn't read registers
	return false;
}

bool AK8963::readRawMag(int16_t &x, int16_t &y, int16_t &z) {
	uint8_t data[8];
	if (_readBytes(Register::ST1, data, 8)) {
		if ((data[0] & (1 << (uint8_t) Bit::DRDY) == 1)) {  // Data is ready
			if ((data[7] & (1 << (uint8_t) Bit::HOFL)) == 0) {  // No overflow
				x = ((int16_t) (((int16_t) data[2] << 8) | (data[1])));
				y = ((int16_t) (((int16_t) data[4] << 8) | (data[3])));
				z = ((int16_t) (((int16_t) data[6] << 8) | (data[5])));
				_overflow = false;
				return true;
			}  // overflow (Eut > 4912uT)
			_overflow = true;
		}  // data not ready
	}  // couldn't read registers
	return false;
}

bool AK8963::readRawMag(int16_t (&mag)[3]) {
	uint8_t data[8];
	if (_readBytes(Register::ST1, data, 8)) {
		if ((data[0] & (1 << (uint8_t) Bit::DRDY) == 1)) {  // Data is ready
			if ((data[7] & (1 << (uint8_t) Bit::HOFL)) == 0) {  // No overflow
				mag[0] = ((int16_t) (((int16_t) data[2] << 8) | (data[1])));
				mag[1] = ((int16_t) (((int16_t) data[4] << 8) | (data[3])));
				mag[2] = ((int16_t) (((int16_t) data[6] << 8) | (data[5])));
				_overflow = false;
				return true;
			}  // overflow (Eut > 4912uT)
			_overflow = true;
		}  // data not ready
	}  // couldn't read registers
	return false;
}

float AK8963::to_ut(int16_t mag) {
	return (float) mag * _sensitivity_ut;
}

bool AK8963::getDataMagRaw(int16_t (&data)[3]){
	return readRawMag(data[0], data[1], data[2]);
}

bool AK8963::isDataMagReady(){
	uint8_t data;
	if (_readBytes(Register::ST1, &data, 1)) {
		if ((data & (1 << (uint8_t) Bit::DRDY) == 1)) {
			return true;
		}
	}
	return false;
}

void AK8963::_initializeSensitivityAdjustment() {
	powerDown();
	_delay(100);
	_writeBits(Register::CNTL1, (uint8_t) FUSE_ROM_ACCESS, Bit::MODE,
			Length::MODE);
	uint8_t data[3];
	_readBytes(Register::ASAX, data, 3);
	_xSensAdj = ((float) data[0] - 128.0f) / 256.0f + 1.0f;
	_ySensAdj = ((float) data[1] - 128.0f) / 256.0f + 1.0f;
	_zSensAdj = ((float) data[2] - 128.0f) / 256.0f + 1.0f;
	powerDown();
}

bool AK8963::_readBit(Register registerAddress, bool* bit, Bit position) {
	uint8_t reg = 0;
	if (_readBytes(registerAddress, &reg)) {
		*bit = reg & (1 << (uint8_t) position);
		return true;
	} else {
		return false;
	}
}
bool AK8963::_writeBit(Register registerAddress, bool bit, Bit position) {
	uint8_t reg = 0;
	if (_readBytes(registerAddress, &reg)) {
		if (bit == true) {
			reg |= (1 << (uint8_t) position);
		} else {
			reg &= ~(1 << (uint8_t) position);
		}
		return _writeBytes(registerAddress, &reg);
	}
	return false;
}

bool AK8963::_readBits(Register registerAddress, uint8_t* bits, Bit firstBit,
		Length length) {
	uint8_t reg;
	if (_readBytes(registerAddress, &reg)) {
		uint8_t shift = ((uint8_t) firstBit - (uint8_t) length) + 1; // shift to correct position
		// bitStart begins from zero, length begins from one
		uint8_t mask = ((1 << (uint8_t) length) - 1) << shift; // holds 1s where the bits will be changed
		reg &= mask;  // clear the other bits
		reg >>= shift;  // shift the target bits to the right
		*bits = reg;
		return true;
	}
	return false;
}
bool AK8963::_writeBits(Register registerAddress, uint8_t bits, Bit firstBit,
		Length length) {
	uint8_t reg;  // register
	if (_readBytes(registerAddress, &reg)) {
		uint8_t shift = ((uint8_t) firstBit - (uint8_t) length) + 1;
		uint8_t mask = ((1 << (uint8_t) length) - 1) << shift;
		bits <<= shift;  // shifts bits to the correct position in the register
		bits &= mask;  // clamp numbers bigger than 2^length
		reg &= ~(mask);  // clear the bits that will be changed
		reg |= bits;  // set bits in register according to bits
		return _writeBytes(registerAddress, &reg);
	}
	return false;
}

bool AK8963::_readBytes(Register registerAddress, uint8_t* bytes, uint8_t size) {
	if (com == nullptr)
		return false;
	return com->readCOM(_address, (uint8_t) registerAddress, bytes, size);
}

bool AK8963::_writeBytes(Register registerAddress, uint8_t* bytes, uint8_t size) {
	if (com == nullptr)
		return false;
	return com->writeCOM(_address, (uint8_t) registerAddress, bytes, size);
}
