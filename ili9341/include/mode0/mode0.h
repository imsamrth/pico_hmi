#ifndef _TEXT_MODE_H
#define _TEXT_MODE_H
#include <stdint.h>
// ARNE-16 palette converted to RGB565 -- https://lospec.com/palette-list/arne-16

#define GPU_TARGET_FPS (6 * 5)
#define GPU_X 320
#define GPU_Y 240
#define GPU_MIN ( GPU_X/2 < GPU_Y ? GPU_X : GPU_Y )
#define GPU_BORDER_X ( GPU_X/2 > GPU_Y ? (GPU_X/2-GPU_Y)/2 : 0 )
#define GPU_BORDER_Y ( GPU_X/2 < GPU_Y ? (GPU_Y-GPU_X/2)/2 : 0 )
#define GPU_REST ( GPU_X/2-2*GPU_BORDER_X )
#define S1_CENTER_X ( GPU_X/4 )
#define S1_CENTER_Y ( GPU_Y/2 )
#define S2_CENTER_X ( 3*GPU_X/4 )
#define S2_CENTER_Y ( GPU_Y/2 )
#define TEXTURE_SIZE 64
#define B_WIDTH 8

typedef struct {
    const uint16_t * fontArray;
    const uint8_t width;
    const uint8_t height;
} FontDescription;

extern const FontDescription fontDescription[4];
extern const uint8_t * IconPack[12];


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
    MODE0_FILE,
    MODE0_WHEEL,
    MODE0_MORE,
    MODE0_MENU,
    MODE0_BT,
    MODE0_BACK,
    MODE0_WEL,
    MODE0_NOTI,
    MODE0_WALL,
    MODE0_FILES_NEW,
    MODE0_BATT,
    MODE0_INFO

} mode0_icon_t;




void GPU_ClearFramebuffers();
void GPU_ClearTexture();
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
void GPU_DrawEmptySquare(uint16_t color, int16_t x, int16_t y, int16_t width, int16_t height);
void GPU_draw_icon(uint16_t color, int16_t x, int16_t y);
void GPU_draw_icon_C(uint16_t color, mode0_icon_t icon,   int16_t x, int16_t y) ;
void GPU_draw_icon_565( int16_t x, int16_t y, int16_t width, int16_t height, mode0_icon_t icon);
void GPU_draw_wallpaper(uint16_t color, mode0_icon_t icon,  int16_t x, int16_t y) ;
void GPU_DrawFilledSquare(uint16_t color, int16_t x, int16_t y, int16_t width, int16_t height);
void GPU_DrawText(uint16_t color, uint8_t  fontDescription, int16_t x, int16_t y, const char * text, uint16_t len);
void GPU_DrawRotatedTexture(int16_t x, int16_t y, float factor, float angle);
// mode0_print_icon(const char *s);
void GPU_DrawLine(uint16_t color, int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void GPU_DrawLetter_L(uint16_t color,  int16_t x, int16_t y, char letter);

void GPU_DrawText_L(uint16_t color,  int16_t x, int16_t y, const char * text, uint16_t len);
void FGPU_DrawLetter(uint16_t color, const FontDescription * fontDescription, int16_t x, int16_t y, char letter);
void FGPU_DrawText(uint16_t color, const FontDescription * fontDescription, int16_t x, int16_t y, const char * text, uint16_t len);


// Won't redraw until the matching _end is invoked.
void mode0_begin();
void mode0_end();

#endif

