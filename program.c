#include <stdio.h>
#include <stdlib.h>

int main() {

	system("avr-gcc -mmcu=atmega88 fixedpoint.c"); //compile avrcode to a.hex
	system("avr-objcopy -j .text -j .data -O ihex a.out a.hex");
	system("sudo avrdude -C ~/avrdude_gpio.conf -c pi_1 -p atmega88p -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xf9:m");
	system("sudo avrdude -C ~/avrdude_gpio.conf -c pi_1 -p atmega88p -U flash:w:a.hex:i");
	
	return 0;
}
