#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>

#define fourPI 0x6488
#define twoPI 0x3244
#define threePItwo 0x25B4
#define PI 0x1923
#define PItwo 0x0C91
#define shift_amount 11
#define shift_mask ((1 << shift_amount) - 1)  //shift_mask = 0x07FF
#define shift_amount_mult 22
#define shift_mask_mult ((1 << shift_amount_mult) - 1)

short short_sin(short x);
short multiply(short a, short b);
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
  OCR2A = 125; // Count up to 42
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
  OCR1A=0;    				//set initial compare at 50%
  ICR1=31; 				// 8 MHz /ICR1/2 = PWM frequency = 100 Hz
  TCCR1A = (1<<COM1A1); 		//zeros in COM1B1,COM1B0,WGM11,WGM10  
  TCCR1B = (1<<WGM13) | (1<<CS11); 	//internal clock, no prescaler , PWM mode 8

}

void pwm_adjust(void) {
	uint16_t y, i;
	

	while(1){
		
		for(i = 0; i < twoPI; i+= 0x0142){
		  //Precalculate value
		  //PORTD |= (1<<PD4);			//Debug and measurement
		  y = 0x0800 + short_sin(i);		//Add 1 (fixed point) to make output between 0 and 2
		  y = multiply(y, 0x7D00);   		//Multiply by 15.75, output will be between 0 and 31.5 (unsigned)
		  //PORTD ^= (1<<PD4);			//Debug and measurement
		  
		  
		  //Wait for interrupt to set flag high then set output
		  while(flag == 0);
		  
		  //shift up by 0.5 then extract unsigned integer (top 5 bits) and use for PWM
		  OCR1A = (unsigned)(y+0x0400) >> 11;
		  flag = 0;
		}
	}
	while(1);
}

short multiply(short a, short b) {

	int multiply_result = (int32_t) a * b >> 11;	//Need to remove factor of 2048 when multiplying	
	return multiply_result;
}


short short_sin(short x) {
	static uint16_t out;
	static const uint16_t coef[] = {0, 0x0800, 0, 0xF800, 0, 0x0800};	//{0,1,0,-1,...} sin coefficients
	static uint16_t xcoef[] = {0x0800, 0, 0, 0, 0, 0, 0, 0};		//used to create array of (x^n)/n!
	static uint16_t ans[8];							//output array, good for checking results
	out=0;
	
	/* --- x is outside of range -3*PI/2 to 3*PI/2 get it in range --- */

	while (x >= (threePItwo)) x -= twoPI;

	while (x <= (-threePItwo)) x += twoPI;


	/* --- Use symmetry if x is inside previous range but outside of -PI/2 to PI/2 ---*/
	if ((x >= (PItwo)) && (x <= (threePItwo))) {
		x -= PI;
		x *= -1;
	}	
	if ((x <= (-PItwo)) && (x >= (-threePItwo))) {
		x += PI;
		x *= -1;
	}
	
	
	/* --- Use fifth order Taylor series to approximate value between -PI/2 to PI/2 --- */

	// Calculate first term of Taylor series
	xcoef[1] = multiply(xcoef[0], x);	//xcoef(i-1)*(x/i)
	ans[1] = multiply(xcoef[1], coef[1]);	//multiply by sine coefficients
	out += ans[1];				//sum first term
	
	// Even terms work out to zero but still need to keep track of factorial
	xcoef[2] = multiply(xcoef[1], (x / 2));	
	
	// Calculate third term of Taylor series
	xcoef[3] = multiply(xcoef[2], (x / 3));
	ans[3] = multiply(xcoef[3], coef[3]);	
	out += ans[3];
	
	// Even terms work out to zero but still need to keep track of factorial
	xcoef[4] = multiply(xcoef[3], (x / 4));
	
	// Calculate fifth term of Taylor series
	xcoef[5] = multiply(xcoef[4], (x / 5));	
	ans[5] = multiply(xcoef[5], coef[5]);			
	out += ans[5];
	
	
	return out;
	
	}

	
