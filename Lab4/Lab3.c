// Lab3.c

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "Timers.h"
#include "Systick.h" 
#include "Speaker.h" 
#include "LCD.h" 
#include "Lab3.h" 
#include "Buttons.h"
#include "PWM.h"

#include <string.h>
#include <stdlib.h>
#include "Timer2.h"
#include "Timer3.h"
#include "UART.h"
#include "esp8266.h"

void PortF_Init(void);

#define PF1   (*((volatile uint32_t *)0x40025008))
#define PF2   (*((volatile uint32_t *)0x40025010))
#define PF3   (*((volatile uint32_t *)0x40025020))
#define PF4   (*((volatile uint32_t *)0x40025040))


// states
#define DEFAULT 0
#define SET_TIME 1
#define SET_ALARM 2 
#define ALARM_PLAYING 3 

// color modes 
#define LIGHT_MODE 0
#define DARK_MODE 1 

// off value for alarm 
#define ALARM_OFF -1

uint32_t minX_global; 
uint32_t maxX_global; 
uint32_t minY_global; 
uint32_t maxY_global; 
uint32_t rangeX; 
uint32_t rangeY; 
uint32_t state; 
uint32_t alarmSet; 
uint32_t colorMode; 
uint32_t time; 
int32_t alarm_time;
uint32_t count; 
uint32_t temp_time; 
uint32_t temp_alarm; 

uint32_t lastV1; 
uint32_t lastV2; 
uint32_t lastV3; 
uint32_t lastV4; 

void switchInit(void){}; 
void timerInit(void){}; 
void enableHeartbeat(void){};

void PortF_Init(void);
	
void Pause(void){
  while(PF4==0x00){ 
    DelayWait10ms(10);
  }
  while(PF4==0x10){
    DelayWait10ms(10);
  }
}


// These 6 variables contain the most recent Blynk to TM4C123 message
// Blynk to TM4C123 uses VP0 to VP15
char serial_buf[64];
char Pin_Number[2]   = "99";       // Initialize to invalid pin number
char Pin_Integer[8]  = "0000";     //
char Pin_Float[8]    = "0.0000";   //
uint32_t pin_num; 
uint32_t pin_int;
 
// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin,uint32_t value){
  if((pin < 70)||(pin > 99)){
    return; // ignore illegal requests
  }
// your account will be temporarily halted 
  // if you send too much data
  ESP8266_OutUDec(pin);       // Send the Virtual Pin #
  ESP8266_OutChar(',');
  ESP8266_OutUDec(value);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
}
 
 
// -------------------------   Blynk_to_TM4C  -----------------------------------
// This routine receives the Blynk Virtual Pin data via the ESP8266 and parses the
// data and feeds the commands to the TM4C.
void Blynk_to_TM4C(void){
	
	// Check to see if a there is data in the RXD buffer
  if(ESP8266_GetMessage(serial_buf)){  // returns false if no message
    
		   
		// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
		// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);  
		
		// Process Input 
		processInput(pin_num, pin_int); 
  }  
}

void SendInformation(void){

	uint32_t hour = time/60; 
	uint32_t min = time%60;
	uint32_t alarm_hour = alarm_time/60; 
	uint32_t alarm_min = alarm_time%60; 

	TM4C_to_Blynk(74, hour);  
	DelayWait10ms(100);
	TM4C_to_Blynk(75, min);  
	DelayWait10ms(100);
	TM4C_to_Blynk(76, alarm_hour);  
	DelayWait10ms(100);
	TM4C_to_Blynk(77, alarm_min);  
 
}



int main(void){
	DisableInterrupts(); 
  PLL_Init(Bus80MHz); 
  PortF_Init();
  ST7735_InitR(INITR_REDTAB);
  Output_Clear();
	ST7735_PlotClear(0, 159);
	ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
	//ST7735_DrawString(0, 2, "Alarm Clock", ST7735_WHITE);
	switchInit(); 
	timerInit(); 
	enableHeartbeat(); 
  Timer0A_Init100HzInt();
  Timer1A_Init();
	PWM0A_Init(40000,10000);
	//Buttons_Init(); 
	
	state = DEFAULT; 
	colorMode = LIGHT_MODE; 
	temp_time = 0; 
  alarm_time = ALARM_OFF; 
	lastV1 = 0; 
	lastV2 = 0; 
	lastV3 = 0; 
	lastV4 = 0; 
	
	ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
  
  Timer2_Init(&Blynk_to_TM4C,800000); 
  // check for receive data from Blynk App every 10ms

  Timer3_Init(&SendInformation,40000000); 
  // Send data back to Blynk App every 1/2 second
  EnableInterrupts();
	
	while(1){
		PF2 ^= 0x04; 
		//DelayWait10ms(100); 
		outputLCD(); 
		if ((alarm_time == time) && ((state == DEFAULT)||(state == ALARM_PLAYING))) {
			state = ALARM_PLAYING;
			outputSpeaker(1);
		}
		else {
			outputSpeaker(0); 
		}
  } 
} 

// PF4 is input
// Make PF2 an output, enable digital I/O, ensure alt. functions off
void PortF_Init(void){ 
	SYSCTL_RCGC2_R |= 0x20; 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
	uint32_t delay = SYSCTL_RCGCGPIO_R;
                                    // 2) no need to unlock PF2, PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x14;      // 4) disable analog function on PF2, PF4
  GPIO_PORTF_PUR_R |= 0x10;         // 5) pullup for PF4
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x14;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x14;         // 7) enable digital port
}


