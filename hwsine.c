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

short fixed_sin(short x);
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
  OCR2A = 255; // Count up to 50
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
	uint16_t y, i;
	

	while(1){
		
		for(i = 0; i < twoPI; i+= 0x0099){
		  //Precalculate value
		  PORTD |= (1<<PD4);			//Debug and measurement
		  y = 0x0800 + fixed_sin(i);		//Add 1 (fixed point) to make output between 0 and 2
		  y = multiply(y, 0x7D00);   		//Multiply by 15.75, output will be between 0 and 31.5 (unsigned)
		  PORTD ^= (1<<PD4);			//Debug and measurement
		  
		  
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

short divide_by_int(short a, int b) {
	
	short result = a / b;
	return result;
}

short fixed_sin(short x) {
	int i;								//iterator
	short coef[] = {0, 0x0800, 0, 0xF800, 0, 0x0800, 0, 0xF800};	//{0,1,0,-1,...} sin coefficients
	short xcoef[] = {0x0800, 0, 0, 0, 0, 0, 0, 0};			//used to create array of (x^n)/n!
	short ans[8];							//output array, good for checking results
	short a = 0, out = 0;
	
	/* --- x is outside of range -3*PI/2 to 3*PI/2 get it in range --- */
	if (x >= (3*PI/2) || x <= (-3*PI/2)) {
		if (x >= (3*PI/2)) {
			while (x >= (3*PI/2)) x -= twoPI;
		}
		else if (x <= (-3*PI/2)) {
			while (x <= (-3*PI/2)) x += twoPI;
		}
	}

	/* --- Use symmetry if x is inside previous range but outside of -PI/2 to PI/2 ---*/
	if ((x >= (PI/2)) && (x <= (3*PI/2))) {
		x -= PI;
		x *= -1;
	}	
	if ((x <= (-PI/2)) && (x >= (-3*PI/2))) {
		x += PI;
		x *= -1;
	}
	
	/* --- Use Taylor series to approximate value between -PI/2 to PI/2 --- */
	for (i = 1; i < 8; i++) {
		xcoef[i] = multiply(xcoef[i-1], divide_by_int(x, i));	//xcoef(i-1)*(x/i)
		ans[i] = multiply(xcoef[i], coef[i]);			//multiply by sine coefs
		out += ans[i];						//sum the ans array for approximation
		}
	return out;
	}
	
	
