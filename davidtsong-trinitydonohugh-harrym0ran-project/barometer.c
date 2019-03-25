/*

  Raspberry Pi I2C
	SDA = gpio pin2
	SCL = gpio pin3

    void i2c_init(void);
    void i2c_read(unsigned slave_address, char *data, int data_length);
    void i2c_write(unsigned slave_address, char *data, int data_length);

		Reference: https://github.com/pololu/lps-arduino/blob/master/LPS.cpp
		Hpa -> Elevation Conversion Tier Estimates https://www.sensorsone.com/altitude-pressure-units-conversion/
		https://www.pololu.com/file/0J623/LPS331AP_AN4159_Hardware_and_software_guidelines.pdf
 */
#include "timer.h"

#include "i2c.h"
#include "barometer.h"
#include <stdint.h>
#include "printf.h"
const unsigned address = 0b1011101; // this is for the barometer
void barometer_write_reg(unsigned char reg, unsigned char v) {
	char data[2] = {reg, v};
	i2c_write(address, data, 2);
}

unsigned barometer_read_reg(unsigned char reg) {
	i2c_write(address, (void*) &reg, 1);
	unsigned char uc = 0;
	i2c_read(address, (void*) &uc, 1);
	return uc;
}

void barometer_init() {
	barometer_write_reg(CTRL_REG1, 0xE0); //turn on and enable
}

unsigned barometer_get_whoami() {
	// should return bb ...
    return barometer_read_reg(WHO_AM_I);
}
float readPressureInchesMillibars()
{

	return (float)readPressureRaw() / 4096;
}
unsigned int readPressureRaw()
{
		unsigned int x;

		uint8_t pxl = barometer_read_reg(PRESS_OUT_XL);
	  uint8_t pl = barometer_read_reg(PRESS_OUT_L);
  	uint8_t ph = barometer_read_reg(PRESS_OUT_H);

		x = (unsigned int)(int8_t)ph << 16 | (uint16_t)pl << 8 | pxl;

		barometer_write_reg(address, PRESS_OUT_XL | (1<<7));
		return x;
}
