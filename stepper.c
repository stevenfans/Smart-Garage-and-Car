// Stepper.c
// Runs on LM4F120/TM4C123
// Provide functions that step the motor once clockwise, step
// once counterclockwise, and initialize the stepper motor
// interface.
// Daniel Valvano
// September 12, 2013
// Modified by Dr. Min He April 28, 2017

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Example 4.1, Programs 4.4, 4.5, and 4.6
   Hardware circuit diagram Figure 4.27

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
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
#include "tm4c123gh6pm.h"
#include "systick.h"

struct State{
  uint8_t Out;     // Output
	//uint8_t Out2;    //second output
  uint8_t Next[4]; // CW/CCW
};
typedef const struct State StateType;

#define right     0     // Next index
#define left 	    1 // Next index
#define back  	  2
#define straight  3

// STATE MACHINE FOR PORT D and PORT C

StateType fsm[12]={
  {	0xCC,{1,3,4,8}}, 
  { 0x66,{2,0,4,8}},
  { 0x33,{3,1,4,8}},
  { 0x11,{0,2,4,8}},
	
  {	0x1C,{0,3,5,8}}, //4
  { 0x36,{0,3,6,8}},//5
  { 0x63,{0,3,7,8}},// 6
  { 0xC1,{0,3,4,8}}, // 7

  {	0xC1,{0,3,4,9}},//8
  { 0x63,{0,3,4,10}}, //9
  { 0x36,{0,3,4,11}}, //10
  { 0x1C,{0,3,4, 8}}, //11
	
};



unsigned char s; // current state

#define STEPPER  (*((volatile uint32_t *)0x4000703C))
#define STEPPER2 (*((volatile uint32_t *)0x400063C0))
	
// MOVING FORWARD
void forward(uint32_t delay){
  s = fsm[s].Next[straight]; // clock wise circular
  STEPPER = fsm[s].Out; // step motor
	STEPPER2 = fsm[s].Out; 

}

// MOVING BACKWARDS
void backwards(uint32_t delay){
  s = fsm[s].Next[back]; // clock wise circular
  STEPPER = fsm[s].Out; // step motor
	STEPPER2 = fsm[s].Out; 

}

// MOVING LEFT
void leftTurn(uint32_t delay){
  s = fsm[s].Next[left]; // clock wise circular
  STEPPER = fsm[s].Out; // step motor
	STEPPER2 = fsm[s].Out; 

}

// MOVING RIGHT
void rightTurn(uint32_t delay){
  s = fsm[s].Next[right]; // clock wise circular
  STEPPER = fsm[s].Out; // step motor
	STEPPER2 = fsm[s].Out; 

}

// Initialize Stepper interface
void Stepper_Init(unsigned int time){
  SYSCTL_RCGCGPIO_R |= 0x08; // 1) activate port D
  SysTick_Init(time);
	s= 0; 
                                    // 2) no need to unlock PD3-0
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO configure PD3-0 as GPIO
  GPIO_PORTD_DIR_R |= 0x0F;   // 5) make PD3-0 out
  GPIO_PORTD_AFSEL_R &= ~0x0F;// 6) disable alt funct on PD3-0
  GPIO_PORTD_DR8R_R |= 0x0F;  // enable 8 mA drive
  GPIO_PORTD_DEN_R |= 0x0F;   // 7) enable digital I/O on PD3-0 
}


// Initialize Stepper interface for port C
void	Init_PortC(unsigned int time){ //1111.0000
  SYSCTL_RCGCGPIO_R |= 0x04;  // 1) activate port C
  SysTick_Init(time);  
	s= 0; 
	
  GPIO_PORTC_AMSEL_R &= ~0xF0;      // 3) disable analog functionality on PC7-4
  GPIO_PORTC_PCTL_R &= ~0xFFFF0000; // 4) GPIO configure PC7-4 as GPIO
  GPIO_PORTC_DIR_R |= 0xF0;   // 5) make PC7-4 out
  GPIO_PORTC_AFSEL_R &= ~0xF0;// 6) disable alt funct on PC7-4
  GPIO_PORTC_DR8R_R |= 0xF0;  // enable 8 mA drive
  GPIO_PORTC_DEN_R |= 0xF0;   // 7) enable digital I/O on PC7-4 
}
