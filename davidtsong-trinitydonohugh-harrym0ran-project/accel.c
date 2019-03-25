/*

  Raspberry Pi I2C
	SDA = gpio pin2
	SCL = gpio pin3

    void i2c_init(void);
    void i2c_read(unsigned slave_addressG, char *data, int data_length);
    void i2c_write(unsigned slave_addressG, char *data, int data_length);

		Reference:https://github.com/pololu/lsm303-arduino/blob/master/LSM303.cpp
 */
#include "timer.h"

#include "i2c.h"
#include "accel.h"
#include <stdint.h>
#include "printf.h"
const unsigned addressG =  0b1101011; // this is for the accel
void accel_write_reg(unsigned char reg, unsigned char v) {
	char data[2] = {reg, v};
	i2c_write(addressG, data, 2);
}

unsigned accel_read_reg(unsigned char reg) {
	i2c_write(addressG, (void*) &reg, 1);
	unsigned char uc = 0;
	i2c_read(addressG, (void*) &uc, 1);
	return uc;
}

void accel_init() {
	accel_write_reg(CTRL_REG4, 0x00); //turn on and enable
	accel_write_reg(CTRL_REG1, 0x6F);
}

unsigned accel_get_whoami() {
	// should return bb ...
    return accel_read_reg(WHO_AM_I);
}

void readaccel(int *x, int *y, int *z)
{
	// Wire.beginTransmission(acc_addressG);
	// assert the MSB of the addressG to get the accelerometer
	// to do slave-transmit subaddressG updating.
	// Wire.write(OUT_X_L_A | (1 << 7));
	accel_write_reg(addressG, OUT_X_L | (1<<7));
	// last_status = Wire.endTransmission();
	// Wire.requestFrom(acc_addressG, (byte)6);

	// unsigned int millis_start = millis();
	// while (Wire.available() < 6) {
	// 	if (io_timeout > 0 && ((unsigned int)millis() - millis_start) > io_timeout)
	// 	{
	// 		did_timeout = true;
	// 		return;
	// 	}
	// }

	uint8_t xla = accel_read_reg(OUT_X_L);
	uint8_t xha = accel_read_reg(OUT_X_H);
	uint8_t yla = accel_read_reg(OUT_Y_L);
	uint8_t yha = accel_read_reg(OUT_Y_H);
	uint8_t zla = accel_read_reg(OUT_Z_L);
	uint8_t zha = accel_read_reg(OUT_Z_H);

	// combine high and low bytes
	// This no longer drops the lowest 4 bits of the readings from the DLH/DLM/DLHC, which are always 0
	// (12-bit resolution, left-aligned). The D has 16-bit resolution
	*x = (int16_t)(xha << 8 | xla);
	*y = (int16_t)(yha << 8 | yla);
	*z = (int16_t)(zha << 8 | zla);
}
