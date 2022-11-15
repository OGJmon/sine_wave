TARGET=sinegen
OBJS=SineGenerator.o
CFLAGS=-mmcu=atmega88 -O2
CC=avr-gcc
OBJCOPY=avr-objcopy
OBFLAGS=-j .text -j .data -O ihex
AVRDUDE=sudo avrdude
AVDFLAGS=-C /home/pi/avrdude_gpio.conf -c pi_1 -p atmega88p -U flash:w:
LIBS=

all: ${TARGET}
	${OBJCOPY} ${OBFLAGS} ${TARGET} ${TARGET}.hex
	${AVRDUDE} ${AVDFLAGS}${TARGET}.hex:i

${TARGET}: ${OBJS}
	${CC} -o ${TARGET} ${CFLAGS} ${OBJS} ${LIBS}

program: ${TARGET}
	${OBJCOPY} ${OBFLAGS} ${TARGET} ${TARGET}.hex
	${AVRDUDE} ${AVDFLAGS}${TARGET}.hex:i
	
clean:
	rm -f ${TARGET} ${OBJS} 
