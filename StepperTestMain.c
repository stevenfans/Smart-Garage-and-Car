// Steven Phan 014358810
// Marcos Arauzo 
// StepperTestMain.c
// stepss on LM4F120/TM4C123
// Test the functions provided by Stepper.c,
// 
// Before connecting a real stepper motor, remember to put the
// proper amount of delay between each CW() or CCW() step.
// Daniel Valvano
// September 12, 2013
// Modified by Min HE

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Example 4.1, Programs 4.4, 4.5, and 4.6
   Hardware circuit diagram Figure 4.27

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, steps or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// PD3 connected to driver for stepper motor coil A
// PD2 connected to driver for stepper motor coil A'
// PD1 connected to driver for stepper motor coil B
// PD0 connected to driver for stepper motor coil B'
#include <stdint.h>
#include "stepper.h"
#include "tm4c123gh6pm.h"

void init_PortF(void);
void init_PortB(void);

#define LIGHT										(*((volatile unsigned long *)0x40025038))
	
#define red   0x02;
#define green 0x08;
#define blue  0x04;

unsigned int button1,button2; 
unsigned int move; 

//flags for 1st button
unsigned int steps; 
unsigned int goForward; 
unsigned int turningLeft;
unsigned int keepGoing;
unsigned int stop,halt, pause, check; // flag to stop the robot

// flags for 2nd button 
unsigned int turningRight; 
unsigned int goBack; 

unsigned int end,end2; 

int main(void){
	
	init_PortB();	
	init_PortF();
	Init_PortC  (40000);	
  Stepper_Init(40000);

  while(1){
		
		// BUTTON 1 CODE GOES HERE: 
	
		if(goForward){
				if(move ==1){
					move = 0;
					LIGHT = blue; 
					forward(0); 
				}		
			}
			
		if(turningLeft){
				if(move ==1){
					move = 0;
					LIGHT = red; 
					leftTurn(0); 
				}		
			}
		
		if(keepGoing){
				if(move ==1){
					move = 0;
					LIGHT = 0x0A; 
					forward(0); 
				}
		}
		
		if(halt){
			LIGHT = 0x06;  // pink
			forward(0); // stop the car
			check += 1; 
		}

		if (end){
			if (check == 1){
			if(move ==1){
				move = 0;
				LIGHT = green; 
				forward(0); 
			}
		}
	}	
		
	////////////////////////////////////////////////
	///////////////////////////////////////////////	
	
		//	BUTTON 2 CODE GOES HERE:
		
			if(goBack){
			 	if(move ==1){
						move = 0;
						LIGHT = green; 
						backwards(0); 
					}			
			}
			
			if (turningRight){
						if(move ==1){
								move = 0;
								LIGHT = blue; 
								rightTurn (0); 
					}
				}
			
			if (end2){
						if(move ==1){
								move = 0;
								LIGHT = red; 
								forward(0); 
							}
				}	
	}
}


void GPIOPortF_Handler(void){
	
	if ((GPIO_PORTF_RIS_R & 0x10)){ // sw1 is pressed
		GPIO_PORTF_ICR_R = 0x10;
		
		button1 = 1; 

	}
	
	if ((GPIO_PORTF_RIS_R & 0x01)){ // sw2 is pressed
		GPIO_PORTF_ICR_R = 0x01;
		button2 = 1; 
	}
}

void GPIOPortB_Handler(void){
		
	GPIO_PORTB_ICR_R = 0x01; // acknologe PB0
	
	if (keepGoing){
		stop = 1; 
	}
	
	if (halt){
		end = 1;
		LIGHT = 0x0C;
	}

}

void SysTick_Handler(void){
 
	if(button1 == 1){
		goForward = 1; 
	}
	
	if(goForward){
		button1 = 0; 
		move += 1; 
		steps += 1;
			if(steps == 4200){
				turningLeft = 1; 
				goForward = 0; 
				move = 0; 
				steps =0;
			}	
		}
	
	if(turningLeft){
		move+=1;
		steps += 1; 
			if(steps == 1000){
				keepGoing = 1;
				turningLeft = 0;
				move = 0;
				steps = 0;
			}
		}
	
	if(keepGoing){
		move+=1;
		if (stop){
			halt = 1;
			keepGoing = 0;
		}
	}
	
	if(end){
		halt = 0; 
		move += 1; 
		steps += 1; 
			if(steps == 2100){
				end = 0; 
				steps = 0;
				move = 0;
				// reset the flags for the
				// ir sensor intterupt
				keepGoing = 0; 
				stop = 0; 
				halt = 0; 
				button1 = 0; 
			}
	}
	
///////////////////////////////////////////////
///////////////////////////////////////////////
	if (button2 == 1){
		  goBack = 1;  
	}		
	
	if(goBack){
		button2 = 0;
		move += 1; 
		steps +=1; 
			if (steps == 2100){
				turningRight = 1; 
				goBack = 0; 
				move = 0;  // set move back to 0
				steps = 0; 
			}
		}
	
	if (turningRight){
		move += 1; 
		steps +=1;		
			if (steps == 1000){
					end2 = 1;
					move = 0; // back to 0
					turningRight = 0; 
					steps = 0;
			}		
	}
	
	if (end2){
		move += 1; 
		steps += 1; 
			if(steps == 4200){
				end2 = 0; 
				move = 0;
				steps = 0;
			}
	}
	
}

void init_PortF(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF3-0 
	GPIO_PORTF_DIR_R = 0x0E;          // 5)PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R &= ~0x1F;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x1F;     //     enable digital I/O on PF4  
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x11;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x11;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R |= 0x40000000;      // (h) enable interrupt 30 in NVIC
}

void init_PortB(void){
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) B clock
  GPIO_PORTB_CR_R = 0x01;           // allow changes to PB0      
  GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTB_PCTL_R = 0x0000000F;   // 4) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R &= 0x01;          // 5) PB0 input 
  GPIO_PORTB_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTB_DEN_R |= 0x01;          // 7) enable digital pins PB0   
																		//0011.1111
	GPIO_PORTB_IS_R &= ~0x01;     // (d) PB0 is edge-sensitive
  GPIO_PORTB_IBE_R |= 0x01;    //     PB0is  both edges
  GPIO_PORTB_ICR_R = 0x01;      // (e) clear flag0 
  GPIO_PORTB_IM_R |= 0x01;      // (f) arm interrupt on PB0
	
	NVIC_PRI0_R = (NVIC_PRI0_R&0XFFFF0FFF) | 0X00008000; // level 4
	NVIC_EN0_R = 0x00000002;      // (h) enable interrupt 2 in NVIC
}

