#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

void init_pwm(void);
void pwm_adjust(void);
void init_timer(void);

static volatile uint8_t flag = 0;

ISR (TIMER2_COMPA_vect)
{
  flag = 1;
  //PORTD^=(1<<PD4);	//Debug and measurement
}

int main()
{
  //update_clock_speed();  //adjust OSCCAL
  init_pwm();  //set up hardware PWM
  init_timer();
  pwm_adjust();
  while(1);
}

void init_timer(void)
{
  
  // **************************************************************
  // ***   Timer 2                                                *
  // **************************************************************
  
  TCCR2A = 2; // set CTC mode for timer 2
  OCR2A = 49; // Count up to 50
  TIFR2 = 0; // clear pending interrupts
  TIMSK2 = 2; // Turning on interrupt
  TCCR2B = 3; // Prescaler set to 8
  
  DDRD |= (1<<PD4); //Set PD4 (pin 6 as output)
  sei(); // enable global interrupts
}

void init_pwm(void)
{
  // **************************************************************
  // ***   Timer 1                                                *
  // **************************************************************
  
  DDRB |= (1<<PB1);  			//set OC1A and OC1B  as an output
  OCR1A=0;    				//initialize register to 0% duty cycle
  ICR1=31; 				// 8 MHz / (ICR1+1) / 2 = PWM frequency = 125 kHz
  TCCR1A = (1<<COM1A1); 		//zeros in COM1B1,COM1B0,WGM11,WGM10  
  TCCR1B = (1<<WGM13) | (1<<CS11); 	//internal clock, no prescaler , PWM mode 8

}

void pwm_adjust(void) {
	float y, i;
	

	while(1){
		
		for(i = 0; i < 6.2832; i+=0.0747){
		  //Precalculate value
		  PORTD |= (1<<PD4);			//Debug and measurement
		  y = 1 + sin(i);		//Add 1 (fixed point) to make output between 0 and 2
		  y = y * 15.75;   		//Multiply by 15.75, output will be between 0 and 31.5 (unsigned)
		  PORTD ^= (1<<PD4);			//Debug and measurement
		  
		  
		  //Wait for interrupt to set flag high then set output
		  while(flag == 0);
		  
		  //shift up by 0.5 then extract unsigned integer (top 5 bits) and use for PWM
		  OCR1A = (int)(y+0.5);
		  flag = 0;
		}
	}
	while(1);
}

	
