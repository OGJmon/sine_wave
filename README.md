# AVRSineGen
Code used to synthesize a sine wave using a PWM output and a low pass filter. 

First of all set up the RPi and AVR using the following tutotial: https://learn.adafruit.com/program-an-avr-or-arduino-using-raspberry-pi-gpio-pins/configuration

Next a low pass filter using a 1uF capacitor and a 1k Ohm resister should be placed on pin 15 (PB1).

Now, download the code and compile and run program.c. There is now a sine wave on pin 15!
