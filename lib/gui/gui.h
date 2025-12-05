/**
 * GUI for WaveShare boards (based on WaveShare's LCD_[0-9]in[0-9]+.h code)
 * 
 * Author: Rainer Keller, HS Esslingen
 */
#ifndef __GUI_H__
#define __GUI_H__

#include "DEV_Config.h"
#include "Fonts/fonts.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WHITE               0xFFFF
#define FONT_BACKGROUND    WHITE

typedef enum {
    ROTATE_0 = 0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
} WIN_ROTATE;

/*
 * Mirror settings, default is MIRROR none
 */
typedef enum {
    MIRROR_NONE = 0,
    MIRROR_HORIZONTAL,
    MIRROR_VERTICAL,
    MIRROR_ORIGIN
} WIN_MIRROR_IMAGE;
#define WIN_MIRROR_DEFAULT MIRROR_NONE

typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL
} WIN_DRAW_FILL;

void gui_image_new (UBYTE * image, UWORD width, UWORD height, WIN_ROTATE rotate, UWORD color);
void gui_image_select (UBYTE * image);

// void gui_set_*

void gui_clear (UWORD color);
void gui_draw_pixel (UWORD x, UWORD y, UWORD color);
// void gui_draw_point (UWORD x, UWORD y, UWORD color, WIN_PIXEL_WIDTH width);
void gui_draw_line (UWORD xstart, UWORD xend, UWORD ystart, UWORD yend, UWORD color);
void gui_draw_rectangle (UWORD xstart, UWORD ystart, UWORD xend, UWORD yend, UWORD color, WIN_DRAW_FILL fill_p);
void gui_draw_circle (UWORD x, UWORD y, UWORD radius, UWORD color, WIN_DRAW_FILL fill_p);

void gui_draw_char(UWORD x, UWORD y, const char ascii_char, sFONT * font, UWORD color, UWORD color_bg);
void gui_draw_string(UWORD x, UWORD y, const char * ascii_str, sFONT * font, UWORD color, UWORD color_bg);

#ifdef __cplusplus
}
#endif

#endif /* __GUI_H__ */