#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include "printf.h"
#include <stdint.h>
// #include "barometer.h"
#include "gyro.h"

void main(void) {

  timer_init();
	uart_init();

  i2c_init();
	// barometer_init();
  gyro_init();
  // barometer_enable_gyroscope();
  // printf("whoami=%x\n", barometer_get_whoami());
  printf("whoami=%x\n", gyro_get_whoami());

  // float p;
  int x,y,z;
	while(1) {
        // printf();
        // p=readPressureInchesMillibars();
        readgyro(&x,&y,&z);

        // 16384 is 1g (1g == 1000mg)
        printf("Gyro: x: %d, y: %d, z: %d \n",x,y,z);
        // printf("Pressure(hpa)=(%f)\n", p);
        timer_delay_ms(200);
	}
}
