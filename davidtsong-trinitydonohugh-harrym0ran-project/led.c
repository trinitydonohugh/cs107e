#include "timer.h"
#include "gpio.h"


void init()
{
	gpio_set_output(10);
	gpio_set_output(9);
	gpio_set_output(11);
	gpio_set_output(5);
	gpio_set_output(6);

}

void main(void)
{
	init();

	while(1)
	{
		gpio_write(10,1);
		gpio_write(9,1);
		gpio_write(11,1);
		gpio_write(5,1);
		gpio_write(6,1);
		//gpio_write(6,1);
		printf("On \n");
		timer_delay(1);
		// gpio_write(2,0);
		// gpio_write(3,0);
		// gpio_write(4,0);
		printf("off \n");
		timer_delay(1);

	}
}