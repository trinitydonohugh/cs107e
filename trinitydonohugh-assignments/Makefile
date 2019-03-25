APPLICATION = apps/console_shell
MY_MODULES = console.o gl.o fb.o

#  shell.o keyboard.o malloc.o backtrace.o printf.o strings.o gpio.o timer.o
#  Can be added to MY_MODULES to use your own code

# MY_MODULES is a list of those library modules (such as gpio.o) 
# for which you intend to use your own code. The reference implementation
# from our libraries will be used for any module you do not name in this list.
# Editing this list allows you to control whether the application being 
# built is using your code or the reference implementation for each module 
# on a per-module basis. Great for testing!
# NOTE: when you name a module in this list, it must provide definitions 
# for all of the symbols in the entire module. For example, if you list 
# gpio.o as one of your modules, your gpio.o must define gpio_set_function, 
# gpio_get_function, ... and so on for all functions declared in the gpio.h 
# header file. If your module forgets to implement any of the needed 
# functions, the linker will bring in gpio.o from reference libpi to 
# resolve the missing definition. But you can't have both gpio.o modules!
# The linker will report multiple definition errors for every function
# that occurs in both your gpio.o and the reference gpio.o. No bueno! 

CFLAGS = -I$(CS107E)/include -g -Wall -Og -std=c99 -ffreestanding 
CFLAGS += -mapcs-frame -fno-omit-frame-pointer -mpoke-function-name -Wpointer-arith
LDFLAGS = -nostdlib -T memmap -L$(CS107E)/lib
LDLIBS = -lpi -lgcc

all : $(APPLICATION).bin $(MY_MODULES)

%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%.elf: %.o $(MY_MODULES) start.o cstart.o
	arm-none-eabi-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

%.o: %.s
	arm-none-eabi-as $(ASFLAGS) $< -o $@

%.list: %.o
	arm-none-eabi-objdump --no-show-raw-insn -d $< > $@

install: $(APPLICATION).bin
	rpi-install.py -p $<

test: tests/test_gl_console.bin
	rpi-install.py -p $<

clean:
	rm -f *.o *.bin *.elf *.list *~
	rm -f apps/*.o apps/*.bin apps/*.elf apps/*.list apps/*~
	rm -f tests/*.o tests/*.bin tests/*.elf tests/*.list tests/*~

.PHONY: all clean install test

.PRECIOUS: %.elf %.o

# empty recipe used to disable built-in rules for native build
%:%.c
%:%.o

define CS107E_ERROR_MESSAGE
ERROR - CS107E environment variable is not set.

Please set it to point to the `cs107e.github.io/cs107e` directory using the
command `export CS107E=<replace with path to your cs107e.github.io directory>/cs107e`.

To have this done automatically, add the above command to your shell
environment configuration file (e.g. ~/.bashrc)
endef

ifndef CS107E
$(error $(CS107E_ERROR_MESSAGE))
endif
