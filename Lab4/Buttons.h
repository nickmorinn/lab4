#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Timers.h" 

extern uint32_t state; 


void readInput(void);
void Buttons_Init(void);
void GPIOPortB_Handler(void);
void DelayWait10ms(uint32_t n);
void processInput(uint32_t button, uint32_t data);
