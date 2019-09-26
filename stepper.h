// Stepper.h

// PD3 connected to driver for stepper motor coil A
// PD2 connected to driver for stepper motor coil A'
// PD1 connected to driver for stepper motor coil B
// PD0 connected to driver for stepper motor coil B'

#include "stdint.h"

// Initialize Stepper interface
void Stepper_Init(unsigned int i);

// initialize the Stepper interface for Port C
void	Init_PortC(unsigned int time);

//forward
void forward(uint32_t delay);

//backwards
void backwards(uint32_t delay);

//left
void leftTurn(uint32_t delay);

//right
void rightTurn(uint32_t delay);




