
#ifndef _TEXT_MODE_H
#define _TEXT_MODE_H

// ARNE-16 palette converted to RGB565 -- https://lospec.com/palette-list/arne-16
typedef enum {
    MODE0_BLACK,
    MODE0_BROWN,
    MODE0_RED,
    MODE0_BLUSH,
    MODE0_GRAY,
    MODE0_DESERT,
    MODE0_ORANGE,
    MODE0_YELLOW,
    MODE0_WHITE,
    MODE0_MIDNIGHT,
    MODE0_DARK_SLATE_GRAY,
    MODE0_GREEN,
    MODE0_YELLOW_GREEN,
    MODE0_BLUE,
    MODE0_PICTON_BLUE,
    MODE0_PALE_BLUE
} mode0_color_t;


typedef enum {
    MODE0_NOTI,
    MODE0_BATT,
    MODE0_MORE,
    MODE0_BT,
    MODE_WEL
} mode0_icon_t;



void mode0_init();
void mode0_clear(mode0_color_t color);
void mode0_draw_screen();
void mode0_draw_region(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void mode0_scroll_vertical(int8_t amount);
void mode0_set_foreground(mode0_color_t color);
void mode0_set_background(mode0_color_t color);
void mode0_set_cursor(uint8_t x, uint8_t y);
uint8_t mode0_get_cursor_x();
uint8_t mode0_get_cursor_y();
void mode0_print(const char *s);
void mode0_write(const char *s, int len);
void mode0_putc(char c);
void mode0_show_cursor();
void mode0_hide_cursor();
void GPU_DrawPixel( uint16_t color, uint16_t x, uint16_t y);
void GPU_render ();
void GPU_DrawEmptyCircle(uint16_t color, int16_t x, int16_t y, int16_t radius);
void GPU_draw_icon(uint16_t color, int16_t x, int16_t y);
void GPU_draw_icon_C(uint16_t color, mode0_icon_t icon,   int16_t x, int16_t y) ;
void GPU_draw_wallpaper(uint16_t color, mode0_icon_t icon,  int16_t x, int16_t y) ;
void GPU_DrawFilledSquare(uint16_t color, int16_t x, int16_t y, int16_t width, int16_t height);
void GPU_DrawText(uint16_t color, uint8_t  fontDescription, int16_t x, int16_t y, const char * text, uint16_t len);
// mode0_print_icon(const char *s);

void GPU_DrawLetter_L(uint16_t color,  int16_t x, int16_t y, char letter);

void GPU_DrawText_L(uint16_t color,  int16_t x, int16_t y, const char * text, uint16_t len);


// Won't redraw until the matching _end is invoked.
void mode0_begin();
void mode0_end();

#endif

