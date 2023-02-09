TARGET=sinegen
OBJS=fixedsine.o
CFLAGS=-mmcu=atmega88 -O2 
CC=avr-gcc
OBJCOPY=avr-objcopy
OBFLAGS=-j .text -j .data -O ihex
AVRDUDE=sudo avrdude
PRGRMFLAGS=-C /home/pi/avrdude_gpio.conf -c pi_1 -p atmega88p -U flash:w:
FUSEFLAGS=-C /home/pi/avrdude_gpio.conf -c pi_1 -p atmega88p -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xf9:m


all: ${TARGET}
	${OBJCOPY} ${OBFLAGS} ${TARGET} ${TARGET}.hex
	${AVRDUDE} ${PRGRMFLAGS}${TARGET}.hex:i

${TARGET}: ${OBJS}
	${CC} -o ${TARGET} ${CFLAGS} ${OBJS} ${LIBS}

program: ${TARGET}
	${OBJCOPY} ${OBFLAGS} ${TARGET} ${TARGET}.hex
	${AVRDUDE} ${PRGRMFLAGS}${TARGET}.hex:i
	
fuse: ${TARGET}
	${AVRDUDE} ${FUSEFLAGS}

clean:
	rm -f ${TARGET} ${TARGET}.hex ${OBJS} 
