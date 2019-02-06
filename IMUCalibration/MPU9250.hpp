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

#pragma once
#include <stdint.h>
#include "ICom.h"
#include "IAcc.h"
#include "IGyro.h"

#define X 0
#define Y 1
#define Z 2

/*!
 @brief MPU9250 class.

 Initial mode: POWER_DOWN.
 I2C max clock: standard mode 100kHz, fast mode 400kHz.
 When changing modes first set POWER_DOWN then 100us wait then mode.
 Sensor overflow: |X|+|Y|+|Z| >= 4912uT.
 CSB pin selects SPI when low and I2C when high.

 Sensitivity:
 0.6 µT/LSB typ. (14-bit)
 0.15µT/LSB typ. (16-bit)
 */
class MPU9250 : public IAcc, public IGyro{
public:
	virtual ~MPU9250();
	/*!
	 @brief Address of MPU9250 enumerator.
	 */
	enum Address
		: uint8_t {
			A_68 = 0x68,  // I2C address
		A_69 = 0x68,  // I2C address
	};

	/*!
	 @brief Constructor.

	 The I2C address of MPU9250 is selected by setting pins CAD0 and
	 CAD1 to VCC or GND. CAD1 changes address' bit 2 and CAD0 changes
	 address' bit 1. VCC sets the corresponding bit to 0 and GND to 1.

	 @note R/W set to 0 is write, 1 is read.

	 @param[in] mpu_address: the I2C address of the MPU9250 device.
	 */
	MPU9250(uint8_t mpu_address);

	/// @name Public methods
	///@{
	/*!
	 @brief Attach read and write functions.

	 In order for this library to be platform independent and to
	 support both I2C and SPI interfaces, it uses callback functions
	 to perform read and write operations.
	 The functions signature is:
	 `bool functionName(uint8_t device, uint8_t registerAddress,
	 uint8_t *data, uint8_t size);` Where 'device'
	 is either the I2C address of the device or some ID of the device
	 when using SPI, 'registerAddress' is the register that will be
	 used for reading/writing, '*data' is an uint8_t pointer holding
	 the data that will be read/written, 'size' is the size of the data
	 in bytes.
	 The functions have to implement burst reads/writes using an SPI or
	 I2C interface. In case of SPI the function also has to select the
	 chip (CS pin).

	 @param[in] *read: Pointer to the read function.
	 @param[in] *write: Pointer to the write function.
	 */
	void attachInterface(ICom *comm);

	/*!
	 @brief Attach delay microseconds function.

	 @param[in] *delay: Pointer to the delay microseconds function.
	 */
	void attachDelay(void (*delay)(uint32_t us));

	/*!
	 @brief Initialize the device.

	 */
	bool initialize();

	/*!
	 @brief Read the device's ID.

	 @details Register: WHO_A_MI - WIA - 0x75.

	 @return Device's ID.
	 */
	uint8_t getWhoAmI();

	/*!
	 @brief Gyro range enumerator.

	 There are four types of ranges of operation:
	 GYRO_RANGE_250DPS - 250
	 GYRO_RANGE_500DPS - 500
	 GYRO_RANGE_1000DPS - 1000
	 GYRO_RANGE_2000DPS - 2000
	 */
	enum GyroRange
		: uint8_t
		{
			GYRO_RANGE_250DPS,
		GYRO_RANGE_500DPS,
		GYRO_RANGE_1000DPS,
		GYRO_RANGE_2000DPS
	};

	/*!
	 @brief Gyro range of operation setter.

	 @see enum GyroRange

	 @param[in] GyroRange: Gyro range of operation.
	 */
	bool setGyroRange(MPU9250::GyroRange range);

	/*!
	 @brief Gyro range of operation getter.

	 @see enum Mode

	 @return Gyro range of operation.
	 @retval enum GyroRange.
	 */
	MPU9250::GyroRange getGyroRange();

	/*!
	 @brief Accel range enumerator.

	 There are four types of ranges of operation:
	 ACCEL_RANGE_2G - 250
	 ACCEL_RANGE_4G - 500
	 ACCEL_RANGE_8G - 1000
	 ACCEL_RANGE_16G - 2000
	 */
	enum AccelRange
		: uint8_t
		{
			ACCEL_RANGE_2G, ACCEL_RANGE_4G, ACCEL_RANGE_8G, ACCEL_RANGE_16G
	};

	/*!
	 @brief Accel range of operation setter.

	 @see enum AccelRange

	 @param[in] AccelRange: Accel range of operation.
	 */
	bool setAccelRange(MPU9250::AccelRange range);

	/*!
	 @brief Accel range of operation getter.

	 @see enum Mode

	 @return Accel range of operation.
	 @retval enum AccelRange.
	 */
	MPU9250::AccelRange getAccelRange();

	/*!
	 @brief reset.

	 All registers are reinitialized.

	 @details Register: CNTL2[SRST]
	 */
	bool reset();

	/*!
	 @brief Read measurement data.

	 Reads the measured magnetic fields along the three axis (x, y and
	 z) from the device and writes the values into the supplied by
	 reference parameters (int16_t &x, int16_t &y and int16_t &z).

	 @details Register: from HXL to HZH

	 @param[out] &x: magnetic field along x-axis.
	 @param[out] &y: magnetic field along y-axis.
	 @param[out] &z: magnetic field along z-axis.

	 @return Status of operation.
	 @retval True if read was successful.
	 */
	bool readRawGyro(int16_t &x, int16_t &y, int16_t &z);

	/*!
	 @brief Read measurement data.

	 Reads the measured magnetic fields along the three axis (x, y and
	 z) from the device and writes the values into the supplied by
	 reference array (float (&mag)[3]).

	 @details Register: from HXL to HZH

	 @param[out] &mag[]: array holding the three magnetic fields.

	 @return Status of operation.
	 @retval True if read was successful.
	 */
	bool readRawGyro(int16_t (&gyr)[3]);

	/*!
	 @brief Read measurement data.

	 Reads the measured magnetic fields along the three axis (x, y and
	 z) from the device and writes the values into the supplied by
	 reference parameters (int16_t &x, int16_t &y and int16_t &z).

	 @details Register: from HXL to HZH

	 @param[out] &x: magnetic field along x-axis.
	 @param[out] &y: magnetic field along y-axis.
	 @param[out] &z: magnetic field along z-axis.

	 @return Status of operation.
	 @retval True if read was successful.
	 */
	bool readGyro(float &x, float &y, float &z);

	/*!
	 @brief Read measurement data.

	 Reads the measured magnetic fields along the three axis (x, y and
	 z) from the device and writes the values into the supplied by
	 reference array (float (&mag)[3]).

	 @details Register: from HXL to HZH

	 @param[out] &mag[]: array holding the three magnetic fields.

	 @return Status of operation.
	 @retval True if read was successful.
	 */
	bool readGyro(float (&gyr)[3]);

	/*!
	 @brief Read measurement data.

	 Reads the measured magnetic fields along the three axis (x, y and
	 z) from the device and writes the values into the supplied by
	 reference parameters (int16_t &x, int16_t &y and int16_t &z).

	 @details Register: from HXL to HZH

	 @param[out] &x: magnetic field along x-axis.
	 @param[out] &y: magnetic field along y-axis.
	 @param[out] &z: magnetic field along z-axis.

	 @return Status of operation.
	 @retval True if read was successful.
	 */
	bool readRawAccel(int16_t &x, int16_t &y, int16_t &z);

	/*!
	 @brief Read measurement data.

	 Reads the measured magnetic fields along the three axis (x, y and
	 z) from the device and writes the values into the supplied by
	 reference array (float (&mag)[3]).

	 @details Register: from HXL to HZH

	 @param[out] &mag[]: array holding the three magnetic fields.

	 @return Status of operation.
	 @retval True if read was successful.
	 */
	bool readRawAccel(int16_t (&acc)[3]);

	/*!
	 @brief Read measurement data.

	 Reads the measured magnetic fields along the three axis (x, y and
	 z) from the device and writes the values into the supplied by
	 reference parameters (int16_t &x, int16_t &y and int16_t &z).

	 @details Register: from HXL to HZH

	 @param[out] &x: magnetic field along x-axis.
	 @param[out] &y: magnetic field along y-axis.
	 @param[out] &z: magnetic field along z-axis.

	 @return Status of operation.
	 @retval True if read was successful.
	 */
	bool readAccel(float &x, float &y, float &z);

	/*!
	 @brief Read measurement data.

	 Reads the measured magnetic fields along the three axis (x, y and
	 z) from the device and writes the values into the supplied by
	 reference array (float (&mag)[3]).

	 @details Register: from HXL to HZH

	 @param[out] &mag[]: array holding the three magnetic fields.

	 @return Status of operation.
	 @retval True if read was successful.
	 */
	bool readAccel(float (&acc)[3]);

	///@}

	bool getDataGyroRaw(int16_t (&data)[3]);
	bool getDataAccRaw(int16_t (&data)[3]);
	bool isDataGyroReady();
	bool isDataAccReady();

private:

	const float base = 32768.0f;
	uint8_t _address = 0;
	uint8_t _device = 0;
	GyroRange _grange = GYRO_RANGE_250DPS;
	AccelRange _arange = ACCEL_RANGE_2G;
	float _gRes = getBaseGyroRange(_grange);
	float _aRes = getBaseAccelRange(_arange);
	ICom *com = nullptr;

	uint8_t reg = 0; // aux register value

	enum DefConfig  // initial config
		: uint8_t {
			// GYRO_CONFIG
		GYRO_FCHOICE_B = 0b00000011,  // Disable filters
		// ACCEL_CONFIG2
		ACCEL_FCHOICE_B = 0b00000100,  // Disable filters
		// PWR_MGMT_1
		CLKSEL = 0b00000001,  // Select automatic clock
		H_RESET = 0b10000000,  // Reset all registers
		// INT_PIN_CFG
		BYPASS_EN = 0b00000010,  // Bypass magnetometer
	};

	enum Register
		: uint8_t {
			SELF_TEST_X_GYRO = 0x00,  // Gyroscope Self-test X-axis (R/W)
		SELF_TEST_Y_GYRO = 0x01,  // Gyroscope Self-test Y-axis (R/W)
		SELF_TEST_Z_GYRO = 0x02,  // Gyroscope Self-test Z-axis (R/W)
		SELF_TEST_X_ACC = 0x0D,  // Accelerometer Self-test X-axis (R/W)
		SELF_TEST_Y_ACC = 0x0E,  // Accelerometer Self-test Y-axis (R/W)
		SELF_TEST_Z_ACC = 0x0F,  // Accelerometer Self-test Z-axis (R/W)
		XG_OFFSET_H = 0x13,  // Gyro Offset X-axis (R/W)
		XG_OFFSET_L = 0x14,  // Gyro Offset X-axis (R/W)
		YG_OFFSET_H = 0x15,  // Gyro Offset Y-axis (R/W)
		YG_OFFSET_L = 0x16,  // Gyro Offset Y-axis (R/W)
		ZG_OFFSET_H = 0x17,  // Gyro Offset Z-axis (R/W)
		ZG_OFFSET_L = 0x18,  // Gyro Offset Z-axis (R/W)
		SMPLRT_DIV = 0x19,  // Sample Rate Divider (R/W)
		CONFIG = 0x1A,  // Configuration (R/W)
		GYRO_CONFIG = 0x1B,  // Gyroscope Configuration (R/W)
		ACCEL_CONFIG = 0x1C,  // Accelerometer Configuration (R/W)
		ACCEL_CONFIG2 = 0x1D,  // Accelerometer Configuration 2 (R/W)
		LP_ACCEL_ODR = 0x1E,  // Low Power Accelerometer ODR Control (R/W)
		WOM_THR = 0x1F,  // Wake-on Motion Threshold (R/W)
		FIFO_EN = 0x23,  // FIFO Enable (R/W)
		I2C_MST_CTRL = 0x24,  // I2C Master Control (R/W)
		I2C_SLV0_ADDR = 0x25,  // I2C Slave 0 Address (R/W)
		I2C_SLV0_REG = 0x26,  // I2C Slave 0 Register (R/W)
		I2C_SLV0_CTRL = 0x27,  // I2C Slave 0 Control (R/W)
		I2C_SLV1_ADDR = 0x28,  // I2C Slave 1 Address (R/W)
		I2C_SLV1_REG = 0x29,  // I2C Slave 1 Register (R/W)
		I2C_SLV1_CTRL = 0x2A,  // I2C Slave 1 Control (R/W)
		I2C_SLV2_ADDR = 0x2B,  // I2C Slave 2 Address (R/W)
		I2C_SLV2_REG = 0x2C,  // I2C Slave 2 Register (R/W)
		I2C_SLV2_CTRL = 0x2D,  // I2C Slave 2 Control (R/W)
		I2C_SLV3_ADDR = 0x2E,  // I2C Slave 3 Address (R/W)
		I2C_SLV3_REG = 0x2F,  // I2C Slave 3 Register (R/W)
		I2C_SLV3_CTRL = 0x30,  // I2C Slave 3 Control (R/W)
		I2C_SLV4_ADDR = 0x31,  // I2C Slave 4 Address (R/W)
		I2C_SLV4_REG = 0x32,  // I2C Slave 4 Register (R/W)
		I2C_SLV4_DO = 0x33,  // I2C Slave 4 Data Out (R/W)
		I2C_SLV4_CTRL = 0x34,  // I2C Slave 4 Control (R/W)
		I2C_SLV4_DI = 0x35,  // I2C Slave 4 DI (R)
		I2C_MST_STATUS = 0x36,  // I2C Master Status (R)
		INT_PIN_CFG = 0x37,  // INT Pin / Bypass Enable Configuration (R/W)
		INT_ENABLE = 0x38,  // Interrupt Enable (R/W)
		INT_STATUS = 0x3A,  // Interrupt Status (R)
		ACCEL_XOUT_H = 0x3B,  // Accelerometer Measurements X-axis (R)
		ACCEL_XOUT_L = 0x3C,  // Accelerometer Measurements X-axis (R)
		ACCEL_YOUT_H = 0x3D,  // Accelerometer Measurements Y-axis (R)
		ACCEL_YOUT_L = 0x3E,  // Accelerometer Measurements Y-axis (R)
		ACCEL_ZOUT_H = 0x3F,  // Accelerometer Measurements Z-axis (R)
		ACCEL_ZOUT_L = 0x40,  // Accelerometer Measurements Z-axis (R)
		TEMP_OUT_H = 0x41,  // Temperature Measurements (R)
		TEMP_OUT_L = 0x42,  // Temperature Measurements (R)
		GYRO_XOUT_H = 0x43,  // Gyroscope Measurements X-axis (R)
		GYRO_XOUT_L = 0x44,  // Gyroscope Measurements X-axis (R)
		GYRO_YOUT_H = 0x45,  // Gyroscope Measurements Y-axis (R)
		GYRO_YOUT_L = 0x46,  // Gyroscope Measurements Y-axis (R)
		GYRO_ZOUT_H = 0x47,  // Gyroscope Measurements Z-axis (R)
		GYRO_ZOUT_L = 0x48,  // Gyroscope Measurements Z-axis (R)
		EXT_SENS_DATA_OO = 0x49,  // External Sensor Data 00 (R)
		EXT_SENS_DATA_O1 = 0x4A,  // External Sensor Data 01 (R)
		EXT_SENS_DATA_O2 = 0x4B,  // External Sensor Data 02 (R)
		EXT_SENS_DATA_O3 = 0x4C,  // External Sensor Data 03 (R)
		EXT_SENS_DATA_O4 = 0x4D,  // External Sensor Data 04 (R)
		EXT_SENS_DATA_O5 = 0x4E,  // External Sensor Data 05 (R)
		EXT_SENS_DATA_O6 = 0x4F,  // External Sensor Data 06 (R)
		EXT_SENS_DATA_O7 = 0x50,  // External Sensor Data 07 (R)
		EXT_SENS_DATA_O8 = 0x51,  // External Sensor Data 08 (R)
		EXT_SENS_DATA_O9 = 0x52,  // External Sensor Data 09 (R)
		EXT_SENS_DATA_1O = 0x53,  // External Sensor Data 10 (R)
		EXT_SENS_DATA_11 = 0x54,  // External Sensor Data 11 (R)
		EXT_SENS_DATA_12 = 0x55,  // External Sensor Data 12 (R)
		EXT_SENS_DATA_13 = 0x56,  // External Sensor Data 13 (R)
		EXT_SENS_DATA_14 = 0x57,  // External Sensor Data 14 (R)
		EXT_SENS_DATA_15 = 0x58,  // External Sensor Data 15 (R)
		EXT_SENS_DATA_16 = 0x59,  // External Sensor Data 16 (R)
		EXT_SENS_DATA_17 = 0x5A,  // External Sensor Data 17 (R)
		EXT_SENS_DATA_18 = 0x5B,  // External Sensor Data 18 (R)
		EXT_SENS_DATA_19 = 0x5C,  // External Sensor Data 19 (R)
		EXT_SENS_DATA_20 = 0x5D,  // External Sensor Data 20 (R)
		EXT_SENS_DATA_21 = 0x5E,  // External Sensor Data 21 (R)
		EXT_SENS_DATA_22 = 0x5F,  // External Sensor Data 22 (R)
		EXT_SENS_DATA_23 = 0x60,  // External Sensor Data 23 (R)
		I2C_SLV0_DO = 0x63,  // I2C Slave 0 Data Out (R/W)
		I2C_SLV1_DO = 0x64,  // I2C Slave 1 Data Out (R/W)
		I2C_SLV2_DO = 0x65,  // I2C Slave 2 Data Out (R/W)
		I2C_SLV3_DO = 0x66,  // I2C Slave 3 Data Out (R/W)
		I2C_MST_DELAY_CTRL = 0x67,  // I2C Master Delay Control (R/W)
		SIGNAL_PATH_RESET = 0x68,  // Signal Path Reset (R/W)
		MOT_DETECT_CTRL = 0x69,  // Accelerometer Interrupt Control (R/W)
		USER_CTRL = 0x6A,  // User Control (R/W)
		PWR_MGMT_1 = 0x6B,  // Power Management 1 (R/W)
		PWR_MGMT_2 = 0x6C,  // Power Management 2 (R/W)
		FIFO_COUNTH = 0x72,  // FIFO Count (R/W)
		FIFO_COUNTL = 0x73,  // FIFO Count (R/W)
		FIFO_R_W = 0x74,  // FIFO Read Write (R/W)
		WHO_A_MI = 0x75,  // Who Am I (R)
		XA_OFFSET_H = 0x77,  // Accelerometer Offset X-axis (R/W)
		XA_OFFSET_L = 0x78,  // Accelerometer Offset X-axis (R/W)
		YA_OFFSET_H = 0x7A,  // Accelerometer Offset Y-axis (R/W)
		YA_OFFSET_L = 0x7B,  // Accelerometer Offset Y-axis (R/W)
		ZA_OFFSET_H = 0x7D,  // Accelerometer Offset Z-axis (R/W)
		ZA_OFFSET_L = 0x7E,  // Accelerometer Offset Z-axis (R/W)
	};
	/*
	 enum : uint8_t {
	 bit0 = 1 << 0, // 0000 0001
	 bit1 = 1 << 1, // 0000 0010
	 bit2 = 1 << 2, // 0000 0100
	 bit3 = 1 << 3, // 0000 1000
	 bit4 = 1 << 4, // 0001 0000
	 bit5 = 1 << 5, // 0010 0000
	 bit6 = 1 << 6, // 0100 0000
	 bit7 = 1 << 7, // 1000 0000
	 };
	 */

	float getBaseGyroRange(GyroRange grange);
	float getBaseAccelRange(AccelRange arange);

	void (*_delay)(uint32_t time) = nullptr;

	bool _writeByte(Register reg, uint8_t* data);
	bool _readByte(Register reg, uint8_t* data);

	bool _readBytes(Register registerAddress, uint8_t* data, uint8_t size = 1);
	bool _writeBytes(Register registerAddress, uint8_t* data, uint8_t size = 1);

};
