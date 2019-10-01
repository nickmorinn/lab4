#include "Speaker.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"


void outputSpeaker(uint32_t i) {

	if(i == 1)
		PWM0_ENABLE_R |= 0x01;
	else
		PWM0_ENABLE_R &= ~0x01;
}
