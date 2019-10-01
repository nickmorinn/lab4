#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"





extern int32_t alarm_time; 
void PWM_Init(uint16_t period, uint16_t high);
void outputSpeaker(uint32_t); 
