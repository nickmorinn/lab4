
#include "LCD.h"
#include "Buttons.h"
#include <limits.h> 

extern uint32_t state; 
extern uint32_t colorMode; 
extern uint32_t time; 
extern int32_t alarm_time;
extern uint32_t temp_time; 
extern uint32_t minX_global; 
extern uint32_t maxX_global;  
extern uint32_t minY_global; 
extern uint32_t maxY_global;  
extern uint32_t rangeX; 
extern uint32_t rangeY; 

#define DEFAULT 0
#define SET_TIME 1 
#define SET_ALARM 2 
#define ALARM_PLAYING 3 
#define ALARM_OFF -1

// 180 points on a circle of radius 2.000
const int32_t CircleXbuf[180] = { 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70, 0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999
};
const int32_t CircleYbuf[180] = {0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999, 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70
};

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY, uint16_t bcolor){
	
	Output_Clear();
	ST7735_PlotClear(minY, maxY); // unsure about this function 
	ST7735_DrawString(0,1,title,ST7735_YELLOW); 
	
	minX_global = minX; 
	maxX_global = maxX; 
	minY_global = minY; 
	maxY_global = maxY; 
	rangeX = maxX - minX; 
	rangeY = maxY - minY; 
	
};	

void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[], uint16_t color){
	
	ST7735_SetTextColor(color);

	for(uint32_t i=0; i<num; i++) {
		ST7735_FillRect(127*(bufX[i] - minX_global)/rangeX, 32 + (127 * (maxY_global - bufY[i]))/rangeY, 2,2,color); 
	}

};


static char *itoa_simple_helper(char *dest, int i) {
  if (i <= -10) {
    dest = itoa_simple_helper(dest, i/10);
  }
  *dest++ = '0' - i%10;
  return dest;
}
char *itoa_simple(char *dest, int i) {
  char *s = dest;
  if (i < 0) {
    *s++ = '-';
  } else {
    i = -i;
  }
  *itoa_simple_helper(s, i) = '\0';
  return dest;
}

uint32_t abs(int32_t x) {
	
	if (x < 0) {
		return (-1*x); 
	}
	else {
		return x; 
	}
}

int sgn(int num){
	if(num < 0) { return -1;}
	else if(num > 0) {return 1;}
	else return 0;
}


void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
	
	
	/*
	int32_t dx = x2-x1; 
	int32_t dy = y2-y1; 
	
	uint32_t abs_dx = abs(dx); 
	uint32_t abs_dy = abs(dy); 
	
	if(abs_dx>abs_dy) {
		
	}
	else {
	}
		
		
		
		
	float m =(float) ((float)dy/dx); 
	int32_t b = y1-m*x1; 
	
	int32_t bufX[100]; 
	int32_t bufY[100]; 
	
	int x = x1; 
	int i = 0;
	while (x < x2) {
		
		bufX[i] = x;
		bufY[i] = m*x+b;
		
		i++;
		x++;
	}
	
	
	
	for (uint32_t i = 0; i < 100; i++) {
		int32_t delta_x = (x1 + i*(dx/100));
		bufX[i] = delta_x; 
		bufY[i] = (int32_t) (m*delta_x+b); 
	}
	
	
	ST7735_XYplot(abs_dx,bufX,bufY, ST7735_BLUE); 
	
	*/
}



void outputTime2(uint32_t hour, uint32_t min) {
	
	
	ST7735_XYplot(180,(int32_t *)CircleXbuf,(int32_t *)CircleYbuf,ST7735_BLUE);
	
	/*
	switch (hour) {
		case 0 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
			break; 
		case 1 : 
			drawLine(82, 62, 62, 100, ST7735_BLUE); 
			break; 
		case 2 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
			break;
		case 3 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
		  break;
		case 4 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
			break; 
		case 5 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
			break; 
		case 6 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
			break;
		case 7 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
		  break;
		case 8 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
			break; 
		case 9 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
			break; 
		case 10 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
			break;
		case 11 : 
			drawLine(62, 42, 62, 100, ST7735_BLUE); 
		  break;
			
			
	}
	
	*/
}


void outputTime(uint32_t hour, uint32_t min) {

	ST7735_DrawString(0,2,"time: ", ST7735_WHITE); 

	switch(hour) {
		case 0 : 
				ST7735_DrawString(6,2,"12:",ST7735_WHITE);
				break; 
		case 1 :
				ST7735_DrawString(6,2,"01:",ST7735_WHITE);
				break; 
		case 2 : 
				ST7735_DrawString(6,2,"02:",ST7735_WHITE);
				break; 
		case 3 :
				ST7735_DrawString(6,2,"03:",ST7735_WHITE);
				break; 
		case 4 : 
				ST7735_DrawString(6,2,"04:",ST7735_WHITE);
				break; 
		case 5 :
				ST7735_DrawString(6,2,"05:",ST7735_WHITE);
				break; 
		case 6 : 
				ST7735_DrawString(6,2,"06:",ST7735_WHITE);
				break; 
		case 7 :
				ST7735_DrawString(6,2,"07:",ST7735_WHITE);
				break; 
		case 8 : 
				ST7735_DrawString(6,2,"08:",ST7735_WHITE);
				break; 
		case 9 :
				ST7735_DrawString(6,2,"09:",ST7735_WHITE);
				break; 
		case 10 : 
				ST7735_DrawString(6,2,"10:",ST7735_WHITE);
				break; 
		case 11 :
				ST7735_DrawString(6,2,"11:",ST7735_WHITE);
				break; 
	}
	char s[100]; 
	if (min < 10) {
		ST7735_DrawString(9,2,"0", ST7735_WHITE); 
		ST7735_DrawString(10,2,itoa_simple(s,min), ST7735_WHITE);
	}
	else {
		ST7735_DrawString(9,2,itoa_simple(s,min), ST7735_WHITE);
	}

};

void outputAlarm(uint32_t hour, uint32_t min) {

	ST7735_DrawString(0,3,"alarm: ", ST7735_WHITE); 

	switch(hour) {
		case 0 : 
				ST7735_DrawString(7,3,"12:",ST7735_WHITE);
				break; 
		case 1 :
				ST7735_DrawString(7,3,"01:",ST7735_WHITE);
				break; 
		case 2 : 
				ST7735_DrawString(7,3,"02:",ST7735_WHITE);
				break; 
		case 3 :
				ST7735_DrawString(7,3,"03:",ST7735_WHITE);
				break; 
		case 4 : 
				ST7735_DrawString(7,3,"04:",ST7735_WHITE);
				break; 
		case 5 :
				ST7735_DrawString(7,3,"05:",ST7735_WHITE);
				break; 
		case 6 : 
				ST7735_DrawString(7,3,"06:",ST7735_WHITE);
				break; 
		case 7 :
				ST7735_DrawString(7,3,"07:",ST7735_WHITE);
				break; 
		case 8 : 
				ST7735_DrawString(7,3,"08:",ST7735_WHITE);
				break; 
		case 9 :
				ST7735_DrawString(7,3,"09:",ST7735_WHITE);
				break; 
		case 10 : 
				ST7735_DrawString(7,3,"10:",ST7735_WHITE);
				break; 
		case 11 :
				ST7735_DrawString(7,3,"11:",ST7735_WHITE);
				break; 
	}
	char s[100]; 
	if (min < 10) {
		ST7735_DrawString(10,3,"0", ST7735_WHITE); 
		ST7735_DrawString(11,3,itoa_simple(s,min), ST7735_WHITE);
	}
	else {
		ST7735_DrawString(10,3,itoa_simple(s,min), ST7735_WHITE);
	}

};




void outputLCD(void){
	
	//uint32_t debug_time[2]; 
	//debug_time[0] = TIMER1_TAR_R;
	
	uint32_t alarm_hour = alarm_time/60; 
	uint32_t alarm_min = alarm_time%60; 
	
	char s[100]; 
	/*
	ST7735_DrawString(0,5,"state: ", ST7735_WHITE); 
	ST7735_DrawString(7,5,itoa_simple(s,state), ST7735_WHITE); 
	*/
	if (state != SET_ALARM) {
		if(alarm_time==-1) {
			ST7735_DrawString(0,3,"ALARM OFF", ST7735_WHITE);
		}
		else {
			ST7735_DrawString(0,3,"alarm: ", ST7735_WHITE); 
			if (alarm_hour < 10) {
				ST7735_DrawString(7, 3, "0", ST7735_WHITE); 
				ST7735_DrawString(8,3,itoa_simple(s,alarm_hour), ST7735_WHITE); 
				ST7735_DrawString(9,3,":", ST7735_WHITE); 
			}
			else {
				ST7735_DrawString(7,3,itoa_simple(s,alarm_hour), ST7735_WHITE); 
				ST7735_DrawString(9,3,":", ST7735_WHITE); 
			}
			
			if (alarm_min < 10) {
				ST7735_DrawString(10,3,"0", ST7735_WHITE); 
				ST7735_DrawString(11,3,itoa_simple(s,alarm_min), ST7735_WHITE); 
			}
			else {
				ST7735_DrawString(10,3,itoa_simple(s,alarm_min), ST7735_WHITE); 
			}
		}
	}
	
	if (state == DEFAULT) {
		uint32_t hour = time/60; 
		uint32_t min = time%60;
		outputTime(hour,min); 
		outputTime2(hour, min); 
		

		ST7735_DrawString(0, 14, "set", ST7735_WHITE); 
		ST7735_DrawString(0, 15, "time", ST7735_WHITE); 
		ST7735_DrawString(5, 14, "set", ST7735_WHITE); 
		ST7735_DrawString(5, 15, "alarm", ST7735_WHITE); 
		ST7735_DrawString(11, 14, "alarm", ST7735_WHITE); 
		ST7735_DrawString(11, 15, "off", ST7735_WHITE); 
		ST7735_DrawString(17, 14, "view", ST7735_WHITE); 
		ST7735_DrawString(17, 15, "mode", ST7735_WHITE); 
		
	}

	else if (state == SET_TIME) {
		
		uint32_t temp_hour = temp_time/60; 
		uint32_t temp_min = temp_time%60; 
		
		outputTime(temp_hour, temp_min); 
		outputTime2(temp_hour, temp_min); 
		
		ST7735_DrawString(0, 15, "okay", ST7735_WHITE); 
		ST7735_DrawString(5, 14, "add", ST7735_WHITE); 
		ST7735_DrawString(5, 15, "hour", ST7735_WHITE); 
		ST7735_DrawString(11, 14, "add", ST7735_WHITE); 
		ST7735_DrawString(11, 15, "min", ST7735_WHITE); 

		ST7735_DrawString(15, 15, "cancel", ST7735_WHITE); 
		
	}

	else if (state == SET_ALARM) {
		
		uint32_t alarm_temp_hour = alarm_time/60; 
		uint32_t alarm_temp_min = alarm_time%60; 
		
		outputAlarm(alarm_temp_hour, alarm_temp_min); 

		// check for button presses and update accordingly 

		ST7735_DrawString(0, 15, "save", ST7735_WHITE); 
		ST7735_DrawString(5, 15, "hours", ST7735_WHITE); 
		ST7735_DrawString(11, 15, "min", ST7735_WHITE); 
		ST7735_DrawString(15, 15, "cancel", ST7735_WHITE); 
		
	}

	else if (state == ALARM_PLAYING) {

		ST7735_DrawString(6,8,"buzz buzz",ST7735_WHITE);
		ST7735_DrawString(0, 14, "turn", ST7735_WHITE); 
		ST7735_DrawString(0, 15, "off", ST7735_WHITE); 
		

	}
	
	//debug_time[1] = TIMER1_TAR_R;

};

