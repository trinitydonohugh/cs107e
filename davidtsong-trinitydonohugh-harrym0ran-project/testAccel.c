#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include "printf.h"
#include <stdint.h>
#include "accel.h"

void main(void) {

  timer_init();
	uart_init();

  i2c_init();
  accel_init();
  printf("whoami=%x\n", accel_get_whoami());

  float p;
  int x,y,z;
	while(1) {
        readaccel(&x,&y,&z);
        printf("accel: x: %d, y: %d, z: %d \n",x,y,z);
        timer_delay_ms(200);
	}
}
