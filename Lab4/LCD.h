#include <stdint.h>
#include "ST7735.h"

void outputLCD(void); 
void outputTime(uint32_t hour, uint32_t second);
char *itoa_simple(char *dest, int i);
static char *itoa_simple_helper(char *dest, int i);
void outputTime2(uint32_t hour, uint32_t min);
void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY, uint16_t bcolor);
void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[], uint16_t color);
void drawLine(int32_t x1, int32_t x2, int32_t y1, int32_t y2, uint32_t color);
uint32_t abs(int32_t x);
