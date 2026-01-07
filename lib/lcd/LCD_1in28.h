/*****************************************************************************
* | File      	:   LCD_1IN28.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2020-12-16
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __LCD_1IN28_H
#define __LCD_1IN28_H	
	
#include "Config/DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>

#define LCD_1IN28_HEIGHT 240
#define LCD_1IN28_WIDTH 240

#define WHITE      0xFFFF
#define BLACK      0x0000
#define BLUE       0x001F
#define BRED       0xF81F
#define GRED       0xFFE0
#define GBLUE      0x07FF
#define RED        0xF800
#define MAGENTA    0xF81F
#define GREEN      0x07E0
#define CYAN       0x7FFF
#define YELLOW     0xFFE0
#define BROWN      0xBC40
#define BRRED      0xFC07
#define GRAY       0x8430
#define DARKBLUE   0x01CF
#define LIGHTBLUE  0x7D7C
#define GRAYBLUE   0x5458
#define LIGHTGREEN 0x841F
#define LGRAY      0xC618
#define LGRAYBLUE  0xA651
#define LBBLUE     0x2B12

#define UBYTE uint8_t
#define UWORD uint16_t

#define HORIZONTAL 0
#define VERTICAL   1

typedef struct{
	UWORD WIDTH;
	UWORD HEIGHT;
	UBYTE SCAN_DIR;
}LCD_1IN28_ATTRIBUTES;
extern LCD_1IN28_ATTRIBUTES LCD_1IN28;

/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_1IN28_Init(UBYTE Scan_dir);
void LCD_1IN28_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);
void LCD_1IN28_Clear(UWORD Color);
void LCD_1IN28_Display(UWORD *Image);
void LCD_1IN28_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN28_DisplayPoint(UWORD X, UWORD Y, UWORD Color);
#endif
