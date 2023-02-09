# Sine Wave Generation
A school project to compare the performance of various ways to approximate a sine wave on a microcontroller. The microcontroller used was an atmega88pa-pu. Code used to synthesize a sine wave using a PWM output and a low pass filter. 

# Instructions
1. First of all set up the RPi and AVR using the following tutotial: https://learn.adafruit.com/program-an-avr-or-arduino-using-raspberry-pi-gpio-pins/configuration

2. Next, set up a low pass filter using a 1uF capacitor and a 1k Ohm resister on a PWM output of the microcontroller. The code uses pin 15(PB1) of the atmega88pa-pu.

3. Now, there are four C programs that can create a sine wave using the AVR PWM output: hwsine.c is an unoptimized fixed point sine approximation, fixed.c is an optimized fixed point sine approximation, floatsine.c is an optimized floating point sine approximation, and lastly mathsine.c uses the math.h sine function. 

# Example results
The blue sine wave was created usinga function generator, and the yellow sine wave was created using the code from this repository along with the PWM output of the atmega88pa-pu microcontroller.

![image](https://user-images.githubusercontent.com/80599443/217859151-da01be7d-df28-48d3-a88a-29ff3fcd1f79.png)
