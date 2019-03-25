//altimeter control 

//record when the hikers starts 

//change LED based on metrics, use if/else 

//put everything in infinite while

#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include "printf.h"
#include <stdint.h>
#include "barometer.h"

void led_init()
{
	gpio_set_output(5);
	gpio_set_output(6);
	gpio_set_output(9);
	gpio_set_output(10);
	gpio_set_output(11);
}

void main(void) 
{

	//initializing
	led_init();
  	timer_init();
	uart_init();
  	i2c_init();
	barometer_init();

	float start_val;
	float cur_val;

	start_val = readPressureInchesMillibars();

	//TARGET AIR PRESSURE
	float end_val = 1020.01;

	float increment_val = (((float)start_val - (float)end_val) / 5.0);
	printf("increment=%f\n", increment_val);

	printf("whoami=%x\n", barometer_get_whoami());

	printf("start(hpa)=(%f)\n", start_val);

	float altMeasurements[10] = {};
	float altTotal;
	volatile float altAverage = 0;
	int altNum = 0;

	while (1) {

		if (altNum < 10) {

			cur_val = readPressureInchesMillibars();
			//altMeasurements[altNum] = cur_val;
			altNum++;
			altTotal += cur_val;
			//printf("Array=(%f)\n", altMeasurements[altNum]);
			timer_delay_ms(100);

		} else {

			altAverage = (altTotal / 10);

			printf("Average=(%f)\n", altAverage);

			if (altAverage <= (end_val)) {
				printf("Level 5 \n");
				gpio_write(5, 1);
				gpio_write(6, 1);
				gpio_write(9, 1);
				gpio_write(10, 1);
				gpio_write(11, 1);
			} else if (altAverage < (start_val - (increment_val * 4))) {
				gpio_write(5, 1);
				gpio_write(6, 1);
				gpio_write(9, 1);
				gpio_write(10, 1);
				gpio_write(11, 0);
			} else if (altAverage < (start_val - (increment_val * 3))) {
				gpio_write(5, 1);
				gpio_write(6, 1);
				gpio_write(9, 1);
				gpio_write(10, 0);
				gpio_write(11, 0);
			} else if (altAverage < (start_val - (increment_val * 2))) {
				gpio_write(5, 1);
				gpio_write(6, 1);
				gpio_write(9, 0);
				gpio_write(10, 0);
				gpio_write(11, 0);
			} else if (altAverage < (start_val - increment_val)) {
				gpio_write(5, 1);
				gpio_write(6, 0);
				gpio_write(9, 0);
				gpio_write(10, 0);
				gpio_write(11, 0);
			} else {
				printf("Level 0 \n");
				gpio_write(5, 0);
				gpio_write(6, 0);
				gpio_write(9, 0);
				gpio_write(10, 0);
				gpio_write(11, 0);
			}

			altNum = 0;
			altTotal = 0;
		}

		

			// if (altAverage <= (start_val - increment_val)) {
			// 	gpio_write(5, 1);
			// 	printf("one");
			// } else {
			// 	gpio_write(5, 0);
			// }

			// if (altAverage <= (start_val - (increment_val * 2))) {
			// 	gpio_write(6, 1);
			// 	printf("two");
			// } else {
			// 	gpio_write(6, 0);
			// }

			// if (altAverage <= (start_val - (increment_val * 3))) {
			// 	gpio_write(9, 1);
			// 	printf("three");
			// } else {
			// 	gpio_write(9, 0);
			// }

			// if (altAverage <= (start_val - (increment_val * 4))) {
			// 	gpio_write(10, 1);
			// 	printf("four");
			// } else {
			// 	gpio_write(10, 0);
			// }

			// if (altAverage <= (end_val)) {
			// 	gpio_write(11, 1);
			// 	printf("five");
			// } else {
			// 	gpio_write(11, 0);
			// }
    }
}

        //if (rounded_p > start_val+1) {
        // 	gpio_write(10,1);
        // } else if (rounded_p > start_val+2) {
        // 	gpio_write(9,1);
        // }
        // // 16384 is 1g (1g == 1000mg)
        // printf("Pressure(hpa)=(%f)\n", p);
        // timer_delay_ms(200);

