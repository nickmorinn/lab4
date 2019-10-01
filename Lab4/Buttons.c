#include "Buttons.h"
#include "Timers.h"
#include "LCD.h"

extern uint32_t state; 
extern uint32_t colorMode; 
extern uint32_t time; 
extern int32_t alarm_time; 

extern uint32_t temp_time; 
extern uint32_t temp_alarm; 

extern uint32_t lastV1; 
extern uint32_t lastV2; 
extern uint32_t lastV3; 
extern uint32_t lastV4; 

#define DEFAULT 0
#define SET_TIME 1
#define SET_ALARM 2 
#define ALARM_PLAYING 3 

#define ALARM_OFF -1 

// color modes 
#define LIGHT_MODE 0
#define DARK_MODE 1 

volatile static unsigned long Touch;     // true on touch
volatile static unsigned long Release;   // true on release
volatile static unsigned long Last0;      // previous
volatile static unsigned long Last1;      // previous
volatile static unsigned long Last2;      // previous
volatile static unsigned long Last3;      // previous

#define PB0        (*((volatile uint32_t *)0x40005004))
#define PB1        (*((volatile uint32_t *)0x40005008))
#define PB2        (*((volatile uint32_t *)0x40005010))
#define PB3        (*((volatile uint32_t *)0x40005020))

void Buttons_Init(void){     
  SYSCTL_RCGCGPIO_R |= 0x00000002; // (a) activate clock for port B
	while((SYSCTL_RCGCGPIO_R & 0x00000002)==0){};           // NOP equivalent 
  GPIO_PORTB_DIR_R &= 0xFFFFFFF0;    // (c) make PB3-0 inputs 
  GPIO_PORTB_AFSEL_R &= 0xFFFFFFF0;  //     set as GPIO 
  GPIO_PORTB_DEN_R |= 0x0000000F;     //     enable digital I/O on PB3-0
	GPIO_PORTB_PCTL_R  &= ~0x0000FFFF; // configure as GPIO
  GPIO_PORTB_AMSEL_R = 0x0;  //    disable analog functionality on PB
  GPIO_PORTB_IS_R &= ~0x0F;     // (d) PB3-0 is edge-sensitive
  GPIO_PORTB_IBE_R |= 0x0F;    //     PB3-0 is both edges
	GPIO_PORTB_ICR_R = 0x0F;      // (e) clear flag
	GPIO_PORTB_IM_R |= 0x0F;      // (f) arm interrupt on PF4
  NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF1FFF)|0x00002000; // (g) priority 1
  NVIC_EN0_R |= 0x00000002;      // (h) enable interrupt 2 in NVIC
	
	
	Touch = 0; 
	Release = 0; 
	Last0 = PB0; 
	Last1 = PB1; 
	Last2 = PB2; 
	Last3 = PB3; 
	
}


void GPIOPortB_Handler(void) {
	GPIO_PORTB_ICR_R = 0x0F; // ack, clear interrupt flag4
	readInput(); 
}

void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}

void processInput(uint32_t button, uint32_t data) {
	
	
	  switch (state) {
    case DEFAULT :
      if ((button == 1) && (data == 0) && (data != lastV1)) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = SET_TIME; 
      }
      else if ((button == 2) && (data == 0) && (data != lastV2)) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
				alarm_time = 0; 
        state = SET_ALARM; 
      }
      else if ((button == 3) && (data == 0) && (data != lastV3)) {
        alarm_time = ALARM_OFF; 
      }
      else if ((button == 4) && (data == 0) && (data != lastV4)) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        if (colorMode == LIGHT_MODE) {
					ST7735_InvertDisplay(1);
          colorMode = DARK_MODE; 
        }
        else {
					ST7735_InvertDisplay(0);
          colorMode = LIGHT_MODE; 
        }
      }
      break;


    case SET_TIME : 
      if ((button == 1) && (data == 0) && (data != lastV1)) {
				Output_Clear();  
		    ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        time = temp_time; 
        state = DEFAULT; 
      }
      else if ((button == 2) && (data == 0) && (data != lastV2)) {
        temp_time += 60; 
      }
      else if ((button == 3) && (data == 0) && (data != lastV3)) {
        temp_time += 1; 
      }
      else if ((button == 4) && (data == 0) && (data != lastV4)) {
				Output_Clear();  
		    ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = DEFAULT; 
      }
      break;


    case SET_ALARM : 
      if ((button == 1) && (data == 0) && (data != lastV1)) {
				Output_Clear();  
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = DEFAULT; 
      }
      else if ((button == 2) && (data == 0) && (data != lastV2)) {
        alarm_time += 60; 
      }
      else if ((button == 3) && (data == 0) && (data != lastV3)) {
        alarm_time += 1;
      }
      else if ((button == 4) && (data == 0) && (data != lastV4)) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = DEFAULT; 
      }
      break;


    case ALARM_PLAYING : 
      if ((button == 1) && (data == 0) && (data != lastV1)) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = DEFAULT; 
        alarm_time = ALARM_OFF; 
      }
      else if ((button == 2) && (data == 0) && (data != lastV2)) {
 
      }
      else if ((button == 3) && (data == 0) && (data != lastV3)) {

      }
      else if ((button == 4) && (data == 0) && (data != lastV4)) {

      }
      break;
  }
}


void readInput(void) {
  
  //temp_time = time;
  //temp_alarm = alarm_time; 
	
  uint32_t buttons = GPIO_PORTB_DATA_R;
	
  // wait for debounce 
	DelayWait10ms(100); 

  switch (state) {
    case DEFAULT :
      if (buttons & 0x01) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = SET_TIME; 
      }
      else if (buttons & 0x02) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
				alarm_time = 0; 
        state = SET_ALARM; 
      }
      else if (buttons & 0x04) {
        alarm_time = ALARM_OFF; 
      }
      else if (buttons & 0x08) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        if (colorMode == LIGHT_MODE) {
					ST7735_InvertDisplay(1);
          colorMode = DARK_MODE; 
        }
        else {
					ST7735_InvertDisplay(0);
          colorMode = LIGHT_MODE; 
        }
      }
      break;


    case SET_TIME : 
      if (buttons & 0x01) {
				Output_Clear();  
		    ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        time = temp_time; 
        state = DEFAULT; 
      }
      else if (buttons & 0x02) {
        temp_time += 60; 
      }
      else if (buttons & 0x04) {
        temp_time += 1; 
      }
      else if (buttons & 0x08) {
				Output_Clear();  
		    ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = DEFAULT; 
      }
      break;


    case SET_ALARM : 
      if (buttons & 0x01) {
				Output_Clear();  
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = DEFAULT; 
      }
      else if (buttons & 0x02) {
        alarm_time += 60; 
      }
      else if (buttons & 0x04) {
        alarm_time += 1;
      }
      else if (buttons & 0x08) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = DEFAULT; 
      }
      break;


    case ALARM_PLAYING : 
      if (buttons & 0x01) {
				Output_Clear(); 
				ST7735_PlotClear(110, 159);
				ST7735_XYplotInit("Alarm Clock",-2500, 2500, -2500, 2500,ST7735_Color565(228,228,228)); // light grey
        state = DEFAULT; 
        alarm_time = ALARM_OFF; 
      }
      else if (buttons & 0x02) {
 
      }
      else if (buttons & 0x04) {

      }
      else if (buttons & 0x08) {

      }
      break;
  }
}


/*
void readInput(void) {
  
  uint32_t temp_time = time;
  uint32_t temp_alarm = alarm_time; 

  uint32_t buttons = GPIO_PORTD_DATA_R;

  // wait for debounce 
	
  switch (state) {
    case DEFAULT :
      if (buttons & 0x01) {
        state = SET_TIME; 
      }
      else if (buttons & 0x02) {
        state = SET_ALARM; 
      }
      else if (buttons & 0x04) {
        state = DEFAULT; 
        alarm_time = ALARM_OFF; 
      }
      else if (buttons & 0x08) {
        if (colorMode == LIGHT_MODE) {
          colorMode = DARK_MODE; 
        }
        else {
          colorMode = LIGHT_MODE; 
        }
      }
      break;


    case SET_TIME : 
      if (buttons & 0x01) {
        time = temp_time; 
        state = DEFAULT; 
      }
      else if (buttons & 0x02) {
        temp_time += 60; 
      }
      else if (buttons & 0x04) {
        temp_time += 1; 
      }
      else if (buttons & 0x08) {
        state = DEFAULT; 
      }
      break;


    case SET_ALARM : 
      if (buttons & 0x01) {
        alarm_time = temp_alarm; 
        state = DEFAULT; 
      }
      else if (buttons & 0x02) {
        temp_alarm += 60; 
      }
      else if (buttons & 0x04) {
        temp_alarm += 1;
      }
      else if (buttons & 0x08) {
        state = DEFAULT; 
      }
      break;


    case ALARM_PLAYING : 
      if (buttons & 0x01) {
        state = DEFAULT; 
        alarm_time = ALARM_OFF; 
      }
      else if (buttons & 0x02) {
 
      }
      else if (buttons & 0x04) {

      }
      else if (buttons & 0x08) {

      }
      break;
  }
	
}

*/
