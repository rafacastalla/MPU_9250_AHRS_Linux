/*
 * i2c.c
 * Author: Jose Simo (2017)
 * Universitat Politecnica de Valencia. AI2-DISCA
 * Creative Commons.
 */

#include "I2C.hpp"

#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

#include "ICom.h"

#define MAX_WRITE_BUFFER_LEN 511

/**
 * Constructor
 */
I2C::I2C(int bus) {
	i2c_fd = 0;
	i2c_bus = bus;
	current_slave = -1;
}

/**
 * Destructor
 */
I2C::~I2C() {
	if (i2c_fd)
		i2c_close();
}

/**
 * Private
 */
bool I2C::i2c_open() {
	char buff[32];
	if (!i2c_fd) {
		sprintf(buff, "/dev/i2c-%d", i2c_bus);
		i2c_fd = ::open(buff, O_RDWR);
		if (i2c_fd < 0) {
			perror("i2c_open()");
			i2c_fd = 0;
			return false;
		}
	}
	return true;
}

/**
 * Private
 */
void I2C::i2c_close() {
	if (i2c_fd) {
		::close(i2c_fd);
		i2c_fd = 0;
		current_slave = 0;
	}
}

/**
 * Private (select slave device)
 *
 */
bool I2C::i2c_select_slave(uint8_t slave_addr) {
	if (current_slave == slave_addr)
		return true;

	if (i2c_open() == false)
		return false;

	if (ioctl(i2c_fd, I2C_SLAVE, slave_addr) < 0) {
		perror("ioctl(I2C_SLAVE)");
		return false;
	}

	current_slave = slave_addr;
	return true;
}

/**
 * Public write
 *
 */
bool I2C::writeCOM(uint8_t device, uint8_t registerAddress, uint8_t *data, uint8_t size) {

/*
	printf("---------------------------------------\n");
	printf("I2C - writeSlaveReg\n");
	printf("Current Slave: %02x\n", current_slave);
	printf("Content Slave: %02x\n", device);
*/
	int result, i;
	unsigned char txBuff[MAX_WRITE_BUFFER_LEN + 1];

	if (size > MAX_WRITE_BUFFER_LEN) {
		printf("I2C::writeSlaveReg: Max write buffer length exceeded.\n");
		return false;
	}

	if (i2c_select_slave(device) == false) {
		printf("Error selecting Slave: Current - %02x - Selected - %02x\n", current_slave, device);
		return false;
	}

	if (size == 0) {
		result = ::write(i2c_fd, &registerAddress, 1);

		if (result < 0) {
			perror("I2C::writeSlaveReg: Write addressing read fail:1");
			return false;
		} else if (result != 1) {
			printf("I2C::writeSlaveReg: Write addressing read fail:2\n");
			return false;
		}
	} else {
		txBuff[0] = registerAddress;

		for (i = 0; i < size; i++)
			txBuff[i + 1] = data[i];

		result = ::write(i2c_fd, txBuff, size + 1);

		if (result < 0) {
			printf("I2C::writeSlaveReg: Write payload fail:3 %d - %s\n", errno, strerror(errno));
			return false;
		} else if (result < (int) size) {
			printf(
					"I2C::writeSlaveReg: Write payload fail:4. Tried %u Wrote %d\n",
					size, result);
			return false;
		}
	}

	return true;
}

/**
 * Public read
 *
 */
bool I2C::readCOM(uint8_t device, uint8_t registerAddress, uint8_t *data, uint8_t size) {
	int tries, result, total;

	if (writeCOM(device, registerAddress, 0, 0) == false)
		return false;

	total = 0;
	tries = 0;

	while (total < size && tries < 5) {
		result = ::read(i2c_fd, data + total, size - total);
		if (result < 0) {
			perror("I2C::readSlaveReg: Read error.");
			break;
		}

		total += result;

		if (total == size)
			break;

		tries++;
		usleep(10000); //sleep_lapse(10000);
	}

	if (total < size)
		return false;

	return true;
}
