#include "Config/DEV_Config.h"
#include "gui/gui.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct win {
    UBYTE * image;
    UBYTE * backup;
    UWORD width;
    UWORD height;
    UWORD width_mem;
    UWORD height_mem;
    UWORD width_byte;
    UWORD height_byte;
    UWORD color;
    UWORD rotate;
    UWORD mirror;
    // SCALE is always 65, aka BGR565 Coloring scheme, aka 2 Bytes per Pixel
} win;

void gui_image_new (UBYTE * image, UWORD width, UWORD height, WIN_ROTATE rotate, UWORD color) {
    win.image = image;
    win.backup = NULL;
    win.width_mem = width;
    win.height_mem = height;
    win.width_byte = width*2;
    win.height_byte = height;
    win.color = color;
    win.rotate = rotate;
    if (rotate == ROTATE_0 || rotate == ROTATE_180) {
        win.width = width;
        win.height = height;
    } else {
        win.width = height;
        win.height = width;
    }
}

void gui_image_select (UBYTE * image) {
    win.image = image;
}

void gui_image_backup (void) {
    UBYTE * tmp;
    if (NULL == win.backup) {
        tmp = malloc (2 * win.width * win.height);
        if (NULL == tmp)
            return;
    }
    win.backup = tmp;
    memcpy (win.backup, win.image, 2 * win.width * win.height);
}

void gui_image_restore(void) {
    if (NULL == win.backup)
        return;
    memcpy (win.image, win.backup, 2 * win.width * win.height);
}

void gui_clear (UWORD color) {
    for (UWORD j = 0; j < win.height_byte; j++) {
        for (UWORD i = 0; i < win.width_byte; i++ ) {
            UDOUBLE offset = i * 2 + j * win.width_byte;
            win.image[offset+0] = (color>>8) & 0xff;
            win.image[offset+1] = color & 0xff;
        }
    }
}

static inline void set_pixel (UWORD x, UWORD y, UWORD color) {
    if (x > win.width || y > win.height)
        return;

    UWORD i, j;
    switch (win.rotate) {
        case 0:
            i = x;
            j = y;
            break;
        case 90:
            i = win.width_mem - y -1;
            j = x;
            break;
        case 180:
            i = win.width_mem - x -1;
            j = win.height_mem - y - 1;
            break;
        case 270:
            i = y;
            j = win.height_mem - x - 1;
            break;
        default:
            return;
    }
    switch(win.mirror) {
        case MIRROR_NONE:
            break;
        case MIRROR_HORIZONTAL:
            i = win.width_mem - i -1;
            break;
        case MIRROR_VERTICAL:
            j = win.height_mem - j -1;
            break;
        case MIRROR_ORIGIN:
            i = win.width_mem - i -1;
            j = win.height_mem - j -1;
            break;
        default:
            return;
    }
    if (i > win.width_mem || j > win.height_mem)
        return;
    UDOUBLE offset = i * 2 + j * win.width_byte;
    win.image[offset+0] = (color >> 8) & 0xff;
    win.image[offset+1] = color & 0xff;
}

void gui_draw_pixel (UWORD x, UWORD y, UWORD color) {
    if (x > win.width || y > win.height)
        return;
    set_pixel(x, y, color);
}

void gui_draw_line (UWORD xstart, UWORD xend, UWORD ystart, UWORD yend, UWORD color) {
    if (xstart > win.width || ystart > win.height ||
        xend > win.width || yend > win.height)
        return;
    UWORD x = xstart;
    UWORD y = ystart;

    UWORD dx = (xend >= xstart) ? (xend - xstart) : (xstart - xend);
    UWORD dy = (yend >= ystart) ? (yend - ystart) : (ystart - yend);

    int x_add = xend >= xstart ? -1 : 1;
    int y_add = yend >= ystart ? -1 : 1;

    int esp = dx + dy;
    
    while (1) {
        gui_draw_pixel(x, y, color);
        if (2 * esp >= dy) {
            if (x == xend)
                break;
            esp += dy;
            x += x_add;
        }
        if (2 * esp <= dx) {
            if (y == yend)
                break;
            esp += dx;
            y += y_add;
        }
    }
}

void gui_draw_rectangle (UWORD xstart, UWORD ystart, UWORD xend, UWORD yend, UWORD color, WIN_DRAW_FILL fill_p) {
    if (fill_p) {
        for (UWORD y = ystart; y < yend; y++)
            gui_draw_line (xstart, y, xend, y, color);
    } else {
        gui_draw_line(xstart, ystart, xend, ystart, color);
        gui_draw_line(xstart, ystart, xstart, yend, color);
        gui_draw_line(xend, ystart, xend, yend, color);
        gui_draw_line(xstart, yend, xend, yend, color);
    }
}

void gui_draw_circle (UWORD x, UWORD y, UWORD radius, UWORD color, WIN_DRAW_FILL fill_p) {
    if (x > win.width || y > win.height)
        return;
    UWORD xpos = 0;
    UWORD ypos = radius;

    int16_t esp  = 3 - (2 * radius);

    int16_t y_count;

    if (fill_p) {
        while (xpos <= ypos) {
            for (y_count = xpos; y_count <= ypos; y_count++) {
                gui_draw_pixel(x + xpos, y + y_count, color);
                gui_draw_pixel(x - xpos, y + y_count, color);

                gui_draw_pixel(x - y_count, y + xpos, color);
                gui_draw_pixel(x - y_count, y - xpos, color);

                gui_draw_pixel(x - xpos, y - y_count, color);
                gui_draw_pixel(x + xpos, y - y_count, color);

                gui_draw_pixel(x + y_count, y - xpos, color);
                gui_draw_pixel(x + y_count, y + xpos, color);
            }
            if (esp < 0)
                esp += 4* xpos + 6;
            else {
                esp += 10 + 4 * (xpos - ypos);
                ypos--;
            }
            xpos++;
        }
    } else {
        while (xpos <= ypos) {
            gui_draw_pixel(x + xpos, y + ypos, color);
            gui_draw_pixel(x - xpos, y + ypos, color);

            gui_draw_pixel(x - ypos, y + xpos, color);
            gui_draw_pixel(x - ypos, y - xpos, color);

            gui_draw_pixel(x - xpos, y - ypos, color);
            gui_draw_pixel(x + xpos, y - ypos, color);

            gui_draw_pixel(x + ypos, y - xpos, color);
            gui_draw_pixel(x + ypos, y + xpos, color);
            if (esp < 0)
                esp += 4* xpos + 6;
            else {
                esp += 10 + 4 * (xpos - ypos);
                ypos--;
            }
            xpos++;
        }
    }

}

void gui_draw_char(UWORD x, UWORD y, const char ascii_char, sFONT * font, UWORD color, UWORD color_bg) {
    if (x > win.width || y > win.height)
        return;

    UDOUBLE char_offset = (ascii_char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char * ptr = &(font->table[char_offset]);

    for (UWORD page = 0; page < font->Height; page++) {
        for (UWORD column = 0; column < font->Width; column++) {
            int set_p;
            set_p = (*ptr & (0x80 >> (column % 8)));
            if (FONT_BACKGROUND == color_bg) {
                if (set_p)
                    set_pixel(x + column, y + page, color);
            } else {
                if (set_p) {
                    set_pixel(x + column, y + page, color);
                } else {
                    set_pixel(x + column, y + page, color_bg);
                }
            }
            if (column % 8 == 7)
                ptr++;
        }
        if (font->Width % 8 != 0)
            ptr++;
    }
}

void gui_draw_string(UWORD x, UWORD y, const char * ascii_str, sFONT * font, UWORD color, UWORD color_bg) {
    UWORD xpos = x;
    UWORD ypos = y;

    if (x > win.width || y > win.height)
        return;
    while (*ascii_str != '\0') {
        if ((xpos + font->Width) > win.width) {
            xpos = x;
            ypos += font->Height;
        }
        if ((ypos + font->Height) > win.height) {
            xpos = x;
            ypos = y;
        }
        gui_draw_char(xpos, ypos, *ascii_str, font, color, color_bg);
        ascii_str++;
        xpos += font->Width;
    }
}
