// mesure pressure for altitude. 
#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include "printf.h"
#include <stdint.h>
#include "barometer.h"

void main(void) {

  timer_init();
	uart_init();

  i2c_init();
	barometer_init();
  printf("whoami=%x\n", barometer_get_whoami());

  float p;
	while(1) {
        p=readPressureInchesMillibars();
        // 16384 is 1g (1g == 1000mg)
        printf("Pressure(hpa)=(%f)\n", p);
        timer_delay_ms(200);
	}
}
