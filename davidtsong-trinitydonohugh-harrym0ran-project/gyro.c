/*

  Raspberry Pi I2C
	SDA = gpio pin2
	SCL = gpio pin3

    void i2c_init(void);
    void i2c_read(unsigned slave_addressA, char *data, int data_length);
    void i2c_write(unsigned slave_addressA, char *data, int data_length);

		Reference:https://github.com/pololu/lsm303-arduino/blob/master/LSM303.cpp
 */
#include "timer.h"

#include "i2c.h"
#include "gyro.h"
#include <stdint.h>
#include "printf.h"
const unsigned addressA = 0b0011101; // this is for the gyro
void gyro_write_reg(unsigned char reg, unsigned char v) {
	char data[2] = {reg, v};
	i2c_write(addressA, data, 2);
}

unsigned gyro_read_reg(unsigned char reg) {
	i2c_write(addressA, (void*) &reg, 1);
	unsigned char uc = 0;
	i2c_read(addressA, (void*) &uc, 1);
	return uc;
}

void gyro_init() {
	gyro_write_reg(CTRL2, 0x00); //turn on and enable
	gyro_write_reg(CTRL1, 0x57);
}

unsigned gyro_get_whoami() {
	// should return bb ...
    return gyro_read_reg(WHO_AM_I);
}

void readgyro(int *x, int *y, int *z)
{
	// Wire.beginTransmission(acc_addressA);
	// assert the MSB of the addressA to get the gyroerometer
	// to do slave-transmit subaddressA updating.
	// Wire.write(OUT_X_L_A | (1 << 7));
	gyro_write_reg(addressA, OUT_X_L_A | (1<<7));
	// last_status = Wire.endTransmission();
	// Wire.requestFrom(acc_addressA, (byte)6);

	// unsigned int millis_start = millis();
	// while (Wire.available() < 6) {
	// 	if (io_timeout > 0 && ((unsigned int)millis() - millis_start) > io_timeout)
	// 	{
	// 		did_timeout = true;
	// 		return;
	// 	}
	// }

	uint8_t xla = gyro_read_reg(OUT_X_L_A);
	uint8_t xha = gyro_read_reg(OUT_X_H_A);
	uint8_t yla = gyro_read_reg(OUT_Y_L_A);
	uint8_t yha = gyro_read_reg(OUT_Y_H_A);
	uint8_t zla = gyro_read_reg(OUT_Z_L_A);
	uint8_t zha = gyro_read_reg(OUT_Z_H_A);

	// combine high and low bytes
	// This no longer drops the lowest 4 bits of the readings from the DLH/DLM/DLHC, which are always 0
	// (12-bit resolution, left-aligned). The D has 16-bit resolution
	*x = (int16_t)(xha << 8 | xla);
	*y = (int16_t)(yha << 8 | yla);
	*z = (int16_t)(zha << 8 | zla);
}
