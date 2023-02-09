#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>

float float_sin(float x);
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
		  y = 1 + float_sin(i);		//Add 1 (fixed point) to make output between 0 and 2
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



float float_sin(float x) {
	static float out;
	static const float coef[] = {0, 1, 0, -1, 0, 1};	//{0,1,0,-1,...} sin coefficients
	static float xcoef[] = {1, 0, 0, 0, 0, 0, 0, 0};		//used to create array of (x^n)/n!
	static float ans[8];							//output array, good for checking results
	out=0;
	

	/* --- x is outside of range -3*PI/2 to 3*PI/2 get it in range --- */

	while (x >= (4.7124)) x -= 6.2832;

	while (x <= (-4.7124)) x += 6.2832;


	/* --- Use symmetry if x is inside previous range but outside of -PI/2 to PI/2 ---*/
	if ((x >= (1.5708)) && (x <= (4.7124))) {
		x -= 3.1416;
		x *= -1;
	}	
	if ((x <= (-1.5708)) && (x >= (-4.7124))) {
		x += 3.1416;
		x *= -1;
	}
	
	
	/* --- Use fifth order Taylor series to approximate value between -PI/2 to PI/2 --- */

	// Calculate first term of Taylor series
	xcoef[1] = xcoef[0] * x;	//xcoef(i-1)*(x/i)
	ans[1] = xcoef[1] * coef[1];	//multiply by sine coefficients
	out += ans[1];				//sum first term
	
	// Even terms work out to zero but still need to keep track of factorial
	xcoef[2] = xcoef[1] * (x / 2);	
	
	// Calculate third term of Taylor series
	xcoef[3] = xcoef[2] * (x / 3);
	ans[3] = xcoef[3] * coef[3];	
	out += ans[3];
	
	// Even terms work out to zero but still need to keep track of factorial
	xcoef[4] = xcoef[3] * (x / 4);
	
	// Calculate fifth term of Taylor series
	xcoef[5] = xcoef[4] * (x / 5);	
	ans[5] = xcoef[5] * coef[5];			
	out += ans[5];
	
	
	return out;
}
	
