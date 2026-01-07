/*****************************************************************************
* | File        :   LCD_1IN28.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                  Used to shield the underlying layers of each master
*                  and enhance portability
*----------------
* | This version:   V1.0
* | Date     :   2020-12-16
* | Info     :   Basic version
*
******************************************************************************/
#include "Config/DEV_Config.h"
#include "lcd/LCD_1in28.h"

#include <stdlib.h>
#include <stdio.h>

LCD_1IN28_ATTRIBUTES LCD_1IN28;


/******************************************************************************
function : Hardware reset
parameter:
******************************************************************************/
static void LCD_1IN28_Reset(void)
{
    gpio_put(LCD_RST_PIN, 1);
    sleep_ms(100);
    gpio_put(LCD_RST_PIN, 0);
    sleep_ms(100);
    gpio_put(LCD_RST_PIN, 1);
    gpio_put(LCD_CS_PIN, 0);
    sleep_ms(100);
}

/******************************************************************************
function : send command
parameter:
     Reg : Command register
******************************************************************************/
static void LCD_1IN28_SendCommand(UBYTE Reg)
{
    gpio_put(LCD_DC_PIN, 0);
    DEV_SPI_WriteByte(Reg);
}

/******************************************************************************
function : send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN28_SendData_8Bit(UBYTE Data)
{
    gpio_put(LCD_DC_PIN, 1);
    DEV_SPI_WriteByte(Data);
}

/******************************************************************************
function : send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN28_SendData_16Bit(UWORD Data)
{
    gpio_put(LCD_DC_PIN, 1);
    DEV_SPI_WriteByte(Data >> 8);
    DEV_SPI_WriteByte(Data);
}

/******************************************************************************
function : Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_1IN28_InitReg(void)
{
    LCD_1IN28_SendCommand(0xEF);
    LCD_1IN28_SendCommand(0xEB);
    LCD_1IN28_SendData_8Bit(0x14);

	/* BEGIN set inter_command HIGH */
    LCD_1IN28_SendCommand(0xFE);     // Inter register enable 1
    LCD_1IN28_SendCommand(0xEF);     // Inter register enable 2
	/* END set inter_command HIGH */

    LCD_1IN28_SendCommand(0xEB);
    LCD_1IN28_SendData_8Bit(0x14);

    LCD_1IN28_SendCommand(0x84);
    LCD_1IN28_SendData_8Bit(0x40);

    LCD_1IN28_SendCommand(0x85);
    LCD_1IN28_SendData_8Bit(0xFF);

    LCD_1IN28_SendCommand(0x86);
    LCD_1IN28_SendData_8Bit(0xFF);

    LCD_1IN28_SendCommand(0x87);
    LCD_1IN28_SendData_8Bit(0xFF);

    LCD_1IN28_SendCommand(0x88);
    LCD_1IN28_SendData_8Bit(0x0A);

    LCD_1IN28_SendCommand(0x89);
    LCD_1IN28_SendData_8Bit(0x21);

    LCD_1IN28_SendCommand(0x8A);
    LCD_1IN28_SendData_8Bit(0x00);

    LCD_1IN28_SendCommand(0x8B);
    LCD_1IN28_SendData_8Bit(0x80);

    LCD_1IN28_SendCommand(0x8C);
    LCD_1IN28_SendData_8Bit(0x01);

    LCD_1IN28_SendCommand(0x8D);
    LCD_1IN28_SendData_8Bit(0x01);

    LCD_1IN28_SendCommand(0x8E);
    LCD_1IN28_SendData_8Bit(0xFF);

    LCD_1IN28_SendCommand(0x8F);
    LCD_1IN28_SendData_8Bit(0xFF);

    /* BEGIN Display function control */
    LCD_1IN28_SendCommand(0xB6);
    LCD_1IN28_SendData_8Bit(0x00);   // Always zero
    LCD_1IN28_SendData_8Bit(0x20);   // GS=0: Gate Output Scan G1->G240
	                                 // SS=1: Source Output Scan S360->S1
	/* END Display function control */

    LCD_1IN28_SendCommand(0x36);     // Memory Access Control
    LCD_1IN28_SendData_8Bit(0x08);   // Set as vertical screen

    LCD_1IN28_SendCommand(0x3A);     // Pixel Format Set
    LCD_1IN28_SendData_8Bit(0x05);   // DBI = 16 Bits/Pixel


    LCD_1IN28_SendCommand(0x90);
    LCD_1IN28_SendData_8Bit(0x08);
    LCD_1IN28_SendData_8Bit(0x08);
    LCD_1IN28_SendData_8Bit(0x08);
    LCD_1IN28_SendData_8Bit(0x08);

    LCD_1IN28_SendCommand(0xBD);
    LCD_1IN28_SendData_8Bit(0x06);

    LCD_1IN28_SendCommand(0xBC);
    LCD_1IN28_SendData_8Bit(0x00);

    LCD_1IN28_SendCommand(0xFF);
    LCD_1IN28_SendData_8Bit(0x60);
    LCD_1IN28_SendData_8Bit(0x01);
    LCD_1IN28_SendData_8Bit(0x04);

    LCD_1IN28_SendCommand(0xC3);     // Power Control 2 for vreg1_vbp_d
    LCD_1IN28_SendData_8Bit(0x13);   // VREG1A = (0x13 + 40)*0.02 + 4
	                                 // VREG1B = (0x13 * 0.02) + 0.3
									 // default is 0x3C
    LCD_1IN28_SendCommand(0xC4);     // Power Control 3 for vreg2
    LCD_1IN28_SendData_8Bit(0x13);   // VREG2A = 0x13 * 0.02 - 4.2
	                                 // VREG2B = 0x13 * 0.02 + 0.3
									 // default is 0x3C

    LCD_1IN28_SendCommand(0xC9);     // Power Control 4 for vrh
    LCD_1IN28_SendData_8Bit(0x22);   // VREG1A = (vrh+vbp_d)*0.02+4
	                                 // VREG2A = (vbn_d-vrh)*0.02-3.4

    LCD_1IN28_SendCommand(0xBE);
    LCD_1IN28_SendData_8Bit(0x11);

    LCD_1IN28_SendCommand(0xE1);
    LCD_1IN28_SendData_8Bit(0x10);
    LCD_1IN28_SendData_8Bit(0x0E);

    LCD_1IN28_SendCommand(0xDF);
    LCD_1IN28_SendData_8Bit(0x21);
    LCD_1IN28_SendData_8Bit(0x0c);
    LCD_1IN28_SendData_8Bit(0x02);

    LCD_1IN28_SendCommand(0xF0);     // Set Gamma 1
    LCD_1IN28_SendData_8Bit(0x45);
    LCD_1IN28_SendData_8Bit(0x09);
    LCD_1IN28_SendData_8Bit(0x08);
    LCD_1IN28_SendData_8Bit(0x08);
    LCD_1IN28_SendData_8Bit(0x26);
    LCD_1IN28_SendData_8Bit(0x2A);

    LCD_1IN28_SendCommand(0xF1);     // Set Gamma 2
    LCD_1IN28_SendData_8Bit(0x43);
    LCD_1IN28_SendData_8Bit(0x70);
    LCD_1IN28_SendData_8Bit(0x72);
    LCD_1IN28_SendData_8Bit(0x36);
    LCD_1IN28_SendData_8Bit(0x37);
    LCD_1IN28_SendData_8Bit(0x6F);


    LCD_1IN28_SendCommand(0xF2);     // Set Gamma 3
    LCD_1IN28_SendData_8Bit(0x45);
    LCD_1IN28_SendData_8Bit(0x09);
    LCD_1IN28_SendData_8Bit(0x08);
    LCD_1IN28_SendData_8Bit(0x08);
    LCD_1IN28_SendData_8Bit(0x26);
    LCD_1IN28_SendData_8Bit(0x2A);

    LCD_1IN28_SendCommand(0xF3);     // Set Gamma 4
    LCD_1IN28_SendData_8Bit(0x43);
    LCD_1IN28_SendData_8Bit(0x70);
    LCD_1IN28_SendData_8Bit(0x72);
    LCD_1IN28_SendData_8Bit(0x36);
    LCD_1IN28_SendData_8Bit(0x37);
    LCD_1IN28_SendData_8Bit(0x6F);

    LCD_1IN28_SendCommand(0xED);
    LCD_1IN28_SendData_8Bit(0x1B);
    LCD_1IN28_SendData_8Bit(0x0B);

    LCD_1IN28_SendCommand(0xAE);
    LCD_1IN28_SendData_8Bit(0x77);

    LCD_1IN28_SendCommand(0xCD);
    LCD_1IN28_SendData_8Bit(0x63);


    LCD_1IN28_SendCommand(0x70);
    LCD_1IN28_SendData_8Bit(0x07);
    LCD_1IN28_SendData_8Bit(0x07);
    LCD_1IN28_SendData_8Bit(0x04);
    LCD_1IN28_SendData_8Bit(0x0E);
    LCD_1IN28_SendData_8Bit(0x0F);
    LCD_1IN28_SendData_8Bit(0x09);
    LCD_1IN28_SendData_8Bit(0x07);
    LCD_1IN28_SendData_8Bit(0x08);
    LCD_1IN28_SendData_8Bit(0x03);

    LCD_1IN28_SendCommand(0xE8);     // Frame Rate
    LCD_1IN28_SendData_8Bit(0x34);   // Display Inversion = 2-column inversion

    LCD_1IN28_SendCommand(0x62);
    LCD_1IN28_SendData_8Bit(0x18);
    LCD_1IN28_SendData_8Bit(0x0D);
    LCD_1IN28_SendData_8Bit(0x71);
    LCD_1IN28_SendData_8Bit(0xED);
    LCD_1IN28_SendData_8Bit(0x70);
    LCD_1IN28_SendData_8Bit(0x70);
    LCD_1IN28_SendData_8Bit(0x18);
    LCD_1IN28_SendData_8Bit(0x0F);
    LCD_1IN28_SendData_8Bit(0x71);
    LCD_1IN28_SendData_8Bit(0xEF);
    LCD_1IN28_SendData_8Bit(0x70);
    LCD_1IN28_SendData_8Bit(0x70);

    LCD_1IN28_SendCommand(0x63);
    LCD_1IN28_SendData_8Bit(0x18);
    LCD_1IN28_SendData_8Bit(0x11);
    LCD_1IN28_SendData_8Bit(0x71);
    LCD_1IN28_SendData_8Bit(0xF1);
    LCD_1IN28_SendData_8Bit(0x70);
    LCD_1IN28_SendData_8Bit(0x70);
    LCD_1IN28_SendData_8Bit(0x18);
    LCD_1IN28_SendData_8Bit(0x13);
    LCD_1IN28_SendData_8Bit(0x71);
    LCD_1IN28_SendData_8Bit(0xF3);
    LCD_1IN28_SendData_8Bit(0x70);
    LCD_1IN28_SendData_8Bit(0x70);

    LCD_1IN28_SendCommand(0x64);
    LCD_1IN28_SendData_8Bit(0x28);
    LCD_1IN28_SendData_8Bit(0x29);
    LCD_1IN28_SendData_8Bit(0xF1);
    LCD_1IN28_SendData_8Bit(0x01);
    LCD_1IN28_SendData_8Bit(0xF1);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x07);

    LCD_1IN28_SendCommand(0x66);
    LCD_1IN28_SendData_8Bit(0x3C);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0xCD);
    LCD_1IN28_SendData_8Bit(0x67);
    LCD_1IN28_SendData_8Bit(0x45);
    LCD_1IN28_SendData_8Bit(0x45);
    LCD_1IN28_SendData_8Bit(0x10);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x00);

    LCD_1IN28_SendCommand(0x67);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x3C);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x01);
    LCD_1IN28_SendData_8Bit(0x54);
    LCD_1IN28_SendData_8Bit(0x10);
    LCD_1IN28_SendData_8Bit(0x32);
    LCD_1IN28_SendData_8Bit(0x98);

    LCD_1IN28_SendCommand(0x74);
    LCD_1IN28_SendData_8Bit(0x10);
    LCD_1IN28_SendData_8Bit(0x85);
    LCD_1IN28_SendData_8Bit(0x80);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(0x4E);
    LCD_1IN28_SendData_8Bit(0x00);

    LCD_1IN28_SendCommand(0x98);
    LCD_1IN28_SendData_8Bit(0x3e);
    LCD_1IN28_SendData_8Bit(0x07);

    LCD_1IN28_SendCommand(0x35);    // Tearing Effect Line ON
    LCD_1IN28_SendCommand(0x21);

    LCD_1IN28_SendCommand(0x11);    // Sleep OUT
    sleep_ms(120);
    LCD_1IN28_SendCommand(0x29);    // Display ON
    sleep_ms(20);
}

/********************************************************************************
function:    Set the resolution and scanning method of the screen
parameter:
        Scan_dir:   Scan direction
********************************************************************************/
static void LCD_1IN28_SetAttributes(UBYTE Scan_dir)
{
    //Get the screen scan direction
    LCD_1IN28.SCAN_DIR = Scan_dir;
    UBYTE MemoryAccessReg;

	/*
	 * MemoryAccessReg:
	 *   D7  D6  D5  D4  D3  D2  D1  D0
	 *   MY  MX  MV  ML  BGR MH  0   0
	 * Where:
	 * MY = Row Adress Order         \ These 3 bits control MCU
	 * MX = Column Adress Order      | to memory write/read
	 * MV = Row/Column Exchange      / direction.
	 * ML = Vertical Refresh Order   | LCD vertical refresh direction control
	 * BGR = RGB-BGR Order           | Color selector switch control
	 *    0 = RGB color filter panel, 1 = BGR color filter panel
	 * MH = Horizontal Refersh Order | LCD horizontal refreshing direction control
	 */

    //Get GRAM and LCD width and height
    if(Scan_dir == HORIZONTAL) {
        LCD_1IN28.HEIGHT = LCD_1IN28_HEIGHT;
        LCD_1IN28.WIDTH = LCD_1IN28_WIDTH;
        MemoryAccessReg = 0xc8;	// MY=set MX=Set; BGR=Set
    } else {
        LCD_1IN28.HEIGHT = LCD_1IN28_WIDTH;
        LCD_1IN28.WIDTH = LCD_1IN28_HEIGHT;
        MemoryAccessReg = 0x68; // MX=Set; MV=Set; BGR=Set
    }

    // Set the read / write scan direction of the frame memory
    LCD_1IN28_SendCommand(0x36);
    LCD_1IN28_SendData_8Bit(MemoryAccessReg);
}

/********************************************************************************
function :    Initialize the lcd
parameter:
********************************************************************************/
void LCD_1IN28_Init(UBYTE Scan_dir)
{
    // Turn on the backlight
    DEV_SET_PWM(100);
    
    // Hardware reset
    LCD_1IN28_Reset();

    // Set the resolution and scanning method of the screen
    LCD_1IN28_SetAttributes(Scan_dir);

    // Set the initialization register
    LCD_1IN28_InitReg();
}

/********************************************************************************
function:    Sets the start position and size of the display area
parameter:
        Xstart  :   X direction Start coordinates
        Ystart  :   Y direction Start coordinates
        Xend    :   X direction end coordinates (inclusive -- max. 239)
        Yend    :   Y direction end coordinates (inclusive -- max. 239)
********************************************************************************/
void LCD_1IN28_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    // Column Address Set -- Set the X coordinates
    LCD_1IN28_SendCommand(0x2A);
    LCD_1IN28_SendData_8Bit(Xstart>>8);
    LCD_1IN28_SendData_8Bit(Xstart);
    LCD_1IN28_SendData_8Bit(Xend>>8);
    LCD_1IN28_SendData_8Bit(Xend);

    // Row Adress Set -- Set the Y coordinates
    LCD_1IN28_SendCommand(0x2B);
    LCD_1IN28_SendData_8Bit(Ystart>>8);
    LCD_1IN28_SendData_8Bit(Ystart);
    LCD_1IN28_SendData_8Bit(Yend>>8);
    LCD_1IN28_SendData_8Bit(Yend);

	// Memory Write
    LCD_1IN28_SendCommand(0x2C);
    gpio_put(LCD_DC_PIN, 1);
}

/******************************************************************************
function :    Clear screen
parameter:
******************************************************************************/
void LCD_1IN28_Clear(UWORD Color)
{
    UWORD j;
    UWORD Image[LCD_1IN28_WIDTH*LCD_1IN28_HEIGHT];

    Color = ((Color<<8)&0xff00)|(Color>>8);

    for (j = 0; j < LCD_1IN28_HEIGHT*LCD_1IN28_WIDTH; j++)
        Image[j] = Color;

    LCD_1IN28_SetWindows(0, 0, LCD_1IN28_WIDTH-1, LCD_1IN28_HEIGHT-1);
    for(j = 0; j < LCD_1IN28_HEIGHT; j++)
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_1IN28_WIDTH], LCD_1IN28_WIDTH*2);
}

/******************************************************************************
function :    Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_1IN28_Display(UWORD *Image)
{
    UWORD j;
    LCD_1IN28_SetWindows(0, 0, LCD_1IN28_WIDTH-1, LCD_1IN28_HEIGHT-1);
    for (j = 0; j < LCD_1IN28_HEIGHT; j++)
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_1IN28_WIDTH], LCD_1IN28_WIDTH*2);
}

void LCD_1IN28_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    LCD_1IN28_SetWindows(Xstart, Ystart, Xend-1, Yend-1);
    for (int j = Ystart; j < Yend; j++) {
        int offset = Xstart + j * LCD_1IN28_WIDTH;
        DEV_SPI_Write_nByte((uint8_t *)&Image[offset], (Xend-Xstart)*2);
    }
}


void LCD_1IN28_DisplayPoint(UWORD X, UWORD Y, UWORD Color)
{
    LCD_1IN28_SetWindows(X, Y, X, Y);
    LCD_1IN28_SendData_16Bit(Color);
}

