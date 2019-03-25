NAME = ledJacket
MODULES = barometer.o accel.o printf.o gyro.o 


# CS107E= ../../../../cs107e
CFLAGS = -I$(CS107E)/include -g -Wall -Og -std=c99 -ffreestanding
LDFLAGS = -nostdlib -T memmap -L$(CS107E)/lib
LDLIBS = -lpiextra -lpi -lgcc


all : $(NAME).bin
%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%.elf: %.o start.o cstart.o $(MODULES)
	arm-none-eabi-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

%.o: %.s
	arm-none-eabi-as $(ASFLAGS) $< -o $@

%.list: %.o
	arm-none-eabi-objdump --no-show-raw-insn -d $< > $@

install: $(NAME).bin
	rpi-install.py -p $<

testGyro: testGyro.bin
	rpi-install.py -p $<

testAccel: testAccel.bin
	rpi-install.py -p $<

testBarometer: testBarometer.bin
	rpi-install.py -p $<

led: led.bin
	rpi-install.py -p $<

main: main.bin
	rpi-install.py -p $<

clean:
	rm -f *.o *.bin *.elf *.list *~

.PHONY: all clean install

.PRECIOUS: %.elf %.o

%:%.c
%:%.o
