/*
 * i2c.h
 * Author: Jose Simo (2017)
 * Universitat Politecnica de Valencia. AI2-DISCA
 * Creative Commons.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "ICom.h"

#define MIN_I2C_BUS 0
#define MAX_I2C_BUS 2

class I2C : public ICom{

private:
	int i2c_bus;
	int i2c_fd;
	uint8_t current_slave;

public:
	I2C(int bus);
	virtual ~I2C();
      
	bool readCOM(uint8_t deviceId, uint8_t address, uint8_t *data, uint8_t size);
	bool writeCOM(uint8_t deviceId, uint8_t address, uint8_t *data, uint8_t size);

private:

	bool i2c_open();
	void i2c_close();
	bool i2c_select_slave(uint8_t slave_addr);
};

