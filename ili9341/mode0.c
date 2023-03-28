#include "pico/stdlib.h"
#include <string.h>
#include "hardware/spi.h"
#include "ili9341/ili9341.h"
#include "mode0/mode0.h"
#include "font/font.h"
#include "icon/icon.h"

/* Character graphics mode */

// Characters are 8x12 -- characters start at (x:1,y:1) and are 5x7 in size, so
// it is possible to not display the full area. This display mode actually treats
// them as 6x10, starting at (x:1,y:0)


// static const uint8_t font_data[95][12] = {
//     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x28, 0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x28, 0x7C, 0x28, 0x7C, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x3C, 0x40, 0x38, 0x04, 0x78, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x60, 0x64, 0x08, 0x10, 0x20, 0x4C, 0x0C, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x20, 0x50, 0x50, 0x20, 0x54, 0x48, 0x34, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x10, 0x54, 0x38, 0x38, 0x54, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x10, 0x20, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x4C, 0x54, 0x64, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x30, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x04, 0x38, 0x40, 0x40, 0x7C, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x04, 0x18, 0x04, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x08, 0x18, 0x28, 0x48, 0x7C, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x7C, 0x40, 0x78, 0x04, 0x04, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x18, 0x20, 0x40, 0x78, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x7C, 0x04, 0x08, 0x10, 0x20, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x44, 0x3C, 0x04, 0x08, 0x30, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x10, 0x20, 0x00, 0x00 },
//     { 0x00, 0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x7C, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x40, 0x20, 0x10, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x04, 0x08, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x04, 0x34, 0x4C, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x28, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x78, 0x44, 0x44, 0x78, 0x44, 0x44, 0x78, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x40, 0x40, 0x40, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x78, 0x44, 0x44, 0x44, 0x44, 0x44, 0x78, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x7C, 0x40, 0x40, 0x70, 0x40, 0x40, 0x7C, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x7C, 0x40, 0x40, 0x70, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x40, 0x4C, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x04, 0x04, 0x04, 0x04, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x6C, 0x54, 0x54, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x78, 0x44, 0x44, 0x78, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x44, 0x44, 0x54, 0x48, 0x34, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x78, 0x44, 0x44, 0x78, 0x50, 0x48, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x44, 0x40, 0x38, 0x04, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x44, 0x44, 0x54, 0x54, 0x6C, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x44, 0x28, 0x10, 0x28, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x44, 0x44, 0x28, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x7C, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7C, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00 },
//     { 0x00, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x38, 0x04, 0x3C, 0x44, 0x3C, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x40, 0x40, 0x58, 0x64, 0x44, 0x44, 0x78, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x38, 0x44, 0x40, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x04, 0x04, 0x34, 0x4C, 0x44, 0x44, 0x3C, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x38, 0x44, 0x78, 0x40, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x28, 0x20, 0x70, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x3C, 0x44, 0x44, 0x4C, 0x34, 0x04, 0x38, 0x00, 0x00 },
//     { 0x00, 0x40, 0x40, 0x58, 0x64, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x08, 0x00, 0x08, 0x08, 0x08, 0x08, 0x48, 0x30, 0x00, 0x00 },
//     { 0x00, 0x40, 0x40, 0x48, 0x50, 0x60, 0x50, 0x48, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x68, 0x54, 0x54, 0x54, 0x54, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x58, 0x64, 0x44, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x38, 0x44, 0x44, 0x64, 0x58, 0x40, 0x40, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x38, 0x44, 0x44, 0x4C, 0x34, 0x04, 0x04, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x58, 0x64, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x3C, 0x40, 0x38, 0x04, 0x78, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x20, 0x70, 0x20, 0x20, 0x20, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x4C, 0x34, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x44, 0x44, 0x54, 0x54, 0x28, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x3C, 0x04, 0x38, 0x00, 0x00 },
//     { 0x00, 0x00, 0x00, 0x7C, 0x08, 0x10, 0x20, 0x7C, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x20, 0x10, 0x10, 0x08, 0x10, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x08, 0x10, 0x10, 0x20, 0x10, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00 },
//     { 0x00, 0x28, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
// };


#define TEXT_HEIGHT 24
#define TEXT_WIDTH 53
#define WIDTH 320
#define HEIGHT 240

#define SWAP_BYTES(color) ((uint16_t)(color>>8) | (uint16_t)(color<<8))


static uint16_t _framebuffer[WIDTH*HEIGHT] = {0};  // GPU VRAM (16bits per color)

static mode0_color_t screen_bg_color = MODE0_BLACK;
static mode0_color_t screen_fg_color = MODE0_WHITE;  // TODO need to store a color per cell
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t screen[TEXT_HEIGHT * TEXT_WIDTH] = { 0 };
static uint8_t colors[TEXT_HEIGHT * TEXT_WIDTH] = { 0 };
static uint8_t show_cursor = 0;

static const FontDescription * f_big = &fontDescription[0];
// static const FontDescription * f_huge = &fontDescription[3];

static int depth = 0;
static uint16_t palette[16] = {
    SWAP_BYTES(0x0000),
    SWAP_BYTES(0x49E5),
    SWAP_BYTES(0xB926),
    SWAP_BYTES(0xE371),
    SWAP_BYTES(0x9CF3),
    SWAP_BYTES(0xA324),
    SWAP_BYTES(0xEC46),
    SWAP_BYTES(0xF70D),
    SWAP_BYTES(0xffff),
    SWAP_BYTES(0x1926),
    SWAP_BYTES(0x2A49),
    SWAP_BYTES(0x4443),
    SWAP_BYTES(0xA664),
    SWAP_BYTES(0x02B0),
    SWAP_BYTES(0x351E),
    SWAP_BYTES(0xB6FD)
};


static uint8_t * icons[5] = {
    &notification_map,
    &battery_map,
    &more_map,
    &bluetooth_map,
    &wel_map
};

void mode0_clear(mode0_color_t color) {
    mode0_begin();
    int size = TEXT_WIDTH*TEXT_HEIGHT;
    memset(screen, 0, size);
    memset(colors, color, size);
    mode0_set_cursor(0, 0);
    mode0_end();
}

void mode0_set_foreground(mode0_color_t color) {
    mode0_begin();
    screen_fg_color = color;
    mode0_end();
}

void mode0_set_background(mode0_color_t color) {
    mode0_begin();
    screen_bg_color = color;
    mode0_end();
}

void mode0_set_cursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
}

void mode0_show_cursor() {
    mode0_begin();
    show_cursor = 1;
    mode0_end();
}

void mode0_hide_cursor() {
    mode0_begin();
    show_cursor = 0;
    mode0_end();
}

uint8_t mode0_get_cursor_x() {
    return cursor_x;
}

uint8_t mode0_get_cursor_y() {
    return cursor_y;
}

void mode0_putc(char c) {
    mode0_begin();
    
    if (cursor_y >= TEXT_HEIGHT) {
        mode0_scroll_vertical(cursor_y-TEXT_HEIGHT+1);
        cursor_y = TEXT_HEIGHT-1;
    }

    int idx = cursor_y*TEXT_WIDTH + cursor_x;
    if (c == '\n') {
        // fill the rest of the line with empty content + the current bg color
        memset(screen+idx, 0, TEXT_WIDTH-cursor_x);
        memset(colors+idx, screen_bg_color, TEXT_WIDTH-cursor_x);
        cursor_y++;
        cursor_x = 0;
    } else if (c == '\r') {
        //cursor_x = 0;
    } else if (c>=32 && c<=127) {
        screen[idx] = c-32;
        colors[idx] = ((screen_fg_color & 0xf) << 4) | (screen_bg_color & 0xf);
        
        cursor_x++;
        if (cursor_x >= TEXT_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    mode0_end();
}

void mode0_print(const char *str) {
    mode0_begin();
    char c;
    while (c = *str++) {
        mode0_putc(c);
    }
    mode0_end();
}

void mode0_write(const char *str, int len) {
    mode0_begin();
    for (int i=0; i<len; i++) {
        mode0_putc(*str++);
    }
    mode0_end();
}

inline void mode0_begin() {
    depth++;
}

inline void mode0_end() {
    if (--depth == 0) {
        mode0_draw_screen();
    }
}

void mode0_draw_region(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    // TODO
    mode0_draw_screen();
}

void mode0_draw_screen() {
    // assert depth == 0?
    depth = 0;
    
    // setup to draw the whole screen
    
    // column address set
    ili9341_set_command(ILI9341_CASET);
    ili9341_command_param(0x00);
    ili9341_command_param(0x00);  // start column
    ili9341_command_param(0x00);
    ili9341_command_param(0xef);  // end column -> 239

    // page address set
    ili9341_set_command(ILI9341_PASET);
    ili9341_command_param(0x00);
    ili9341_command_param(0x00);  // start page
    ili9341_command_param(0x01);
    ili9341_command_param(0x3f);  // end page -> 319

    // start writing
    ili9341_set_command(ILI9341_RAMWR);

    uint16_t buffer[6*240];  // 'amount' pixels wide, 240 pixels tall

    int screen_idx = 0;
    for (int x=0; x<TEXT_WIDTH; x++) {
        // create one column of screen information
        
        uint16_t *buffer_idx = buffer;
        
        for (int bit=0; bit<6; bit++) {
            uint8_t mask = 64>>bit;
            for (int y=TEXT_HEIGHT-1; y>=0; y--) {
                uint8_t character = screen[y*53+x];
                uint16_t fg_color = palette[colors[y*53+x] >> 4];
                uint16_t bg_color = palette[colors[y*53+x] & 0xf];

                if (show_cursor && (cursor_x == x) && (cursor_y == y)) {
                    bg_color = MODE0_GREEN;
                }
                                
                const uint8_t* pixel_data = font_data[character];
                
                // draw the character into the buffer
                for (int j=10; j>=1; j--) {
                    //*buffer_idx++ = (pixel_data[j] & mask) ? fg_color : bg_color;
                    *buffer_idx++ = bg_color ;
                    //(pixel_data[j] & mask) ?  : bg_color;
                }
            }
        }
        
        // now send the slice
        ili9341_write_data(buffer, 6*240*2);
    }
    
    uint16_t extra_buffer[2*240] = { 0 };
    ili9341_write_data(extra_buffer, 2*240*2);

}

void mode0_scroll_vertical(int8_t amount) {
    mode0_begin();

    
    if (amount > 0) {
        int size1 = TEXT_WIDTH*amount;
        int size2 = TEXT_WIDTH*TEXT_HEIGHT - size1;
        
        memmove(screen, screen+size1, size2);
        memmove(colors, colors+size1, size2);
        memset(screen+size2, 0, size1);
        memset(colors+size2, screen_bg_color, size1);
    } else if (amount < 0) {
        amount = -amount;
        int size1 = TEXT_WIDTH*amount;
        int size2 = TEXT_WIDTH*TEXT_HEIGHT - size1;

        memmove(screen+size1, screen, size2);
        memmove(colors+size1, colors, size2);
        memset(screen, 0, size1);
        memset(colors, screen_bg_color, size1);
    }
    
    mode0_end();
}

void mode0_init() {
    stdio_init_all();

    ili9341_init();
}

void mode0_print_icon(const char *str) {
    mode0_begin();
    char c;

    c = *str;
    mode0_puti(c);
    // while (c = *str++) {
    //     mode0_puti(c);
    // }
    if (--depth == 0) {
        mode0_draw_icon();
    }
}

void mode0_puti(char c) {
    mode0_begin();
    
    if (cursor_y >= TEXT_HEIGHT) {
        mode0_scroll_vertical(cursor_y-TEXT_HEIGHT+1);
        cursor_y = TEXT_HEIGHT-1;
    }

    int idx = cursor_y*TEXT_WIDTH + cursor_x;
    if (c == '\n') {
        // fill the rest of the line with empty content + the current bg color
        memset(screen+idx, 0, TEXT_WIDTH-cursor_x);
        memset(colors+idx, screen_bg_color, TEXT_WIDTH-cursor_x);
        cursor_y++;
        cursor_x = 0;
    } else if (c == '\r') {
        //cursor_x = 0;
    } else if (c>=32 && c<=127) {
        screen[idx] = c%5;
        colors[idx] = ((screen_fg_color & 0xf) << 4) | (screen_bg_color & 0xf);
        
        cursor_x++;
        if (cursor_x >= TEXT_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    if (--depth == 0) {
        mode0_draw_icon();
    }
}

// DRAW ICON 

void mode0_draw_icon() {
    // assert depth == 0?
    depth = 0;
    
    // setup to draw the whole screen
    
    // column address set
    ili9341_set_command(ILI9341_CASET);
    ili9341_command_param(0x00);
    ili9341_command_param(0x00);  // start column
    ili9341_command_param(0x00);
    ili9341_command_param(0xef);  // end column -> 239

    // page address set
    ili9341_set_command(ILI9341_PASET);
    ili9341_command_param(0x00);
    ili9341_command_param(0x00);  // start page
    ili9341_command_param(0x01);
    ili9341_command_param(0x3f);  // end page -> 319

    // start writing
    ili9341_set_command(ILI9341_RAMWR);

    uint16_t buffer[6*240];  // 'amount' pixels wide, 240 pixels tall

    int screen_idx = 0;

    for (int x=0; x<4; x++) {
        // create one column of screen information
        
        uint16_t *buffer_idx = buffer;
        
        for (int bit=0; bit<6; bit++) {
            //uint8_t mask = 64>>bit;
            for (int y=239; y>=24; y--) {
                *buffer_idx++ = MODE0_BLACK;
            }

            for(int y = 23; y>=0; y--){

                                
                const uint8_t pixel_data = icon_pack[y*24 + x*6 + bit];
                *buffer_idx++ = pixel_data;
                // draw the character into the buffer
                // for (int j=24; j>=1; j--) {
                //     *buffer_idx++ = (pixel_data[j] & mask) ? fg_color : bg_color;
                //     //*buffer_idx++ = bg_color ;
                //     //(pixel_data[j] & mask) ?  : bg_color;
                // }
            }
        }
        
        // now send the slice
        ili9341_write_data(buffer, 6*240*2);
    }

    for (int x=4; x<53; x++) {
        // create one column of screen information
        
        uint16_t *buffer_idx = buffer;
        
        for (int bit=0; bit<6; bit++) {
            //uint8_t mask = 64>>bit;
            for (int y=239; y>=0; y--) {
                *buffer_idx++ = MODE0_BLACK;
            }

            // for(int y = 23; y>=0; y--){

                                
            //     const uint8_t pixel_data = icon_pack[y*24 + x*6 + bit];
            //     *buffer_idx++ = pixel_data;
            //     // draw the character into the buffer
            //     // for (int j=24; j>=1; j--) {
            //     //     *buffer_idx++ = (pixel_data[j] & mask) ? fg_color : bg_color;
            //     //     //*buffer_idx++ = bg_color ;
            //     //     //(pixel_data[j] & mask) ?  : bg_color;
            //     // }
            // }
        }
        
        // now send the slice
        ili9341_write_data(buffer, 6*240*2);
    }
    
    uint16_t extra_buffer[2*240] = { 0 };
    ili9341_write_data(extra_buffer, 2*240*2);

}

// DRAW GPU VECTORS

void GPU_DrawPixel( uint16_t color, uint16_t x, uint16_t y)
{
    if (x >= WIDTH|| y >= HEIGHT)
        return;
    uint16_t * pix = &_framebuffer[(WIDTH-x-1)*HEIGHT+y];
    (*pix) = (uint16_t) palette[color];
}

void GPU_DrawLine(uint16_t color, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    // Bresenhams line algorithm
    x0 = (x0 >= WIDTH) ? WIDTH-1 : x0;
    x1 = (x1 >= WIDTH) ? WIDTH-1 : x1;
    y0 = (y0 >= HEIGHT) ? HEIGHT-1 : y0;
    y1 = (y1 >= HEIGHT) ? HEIGHT-1 : y1;
    int16_t dx =  x1>x0 ? x1-x0 : x0-x1;
    int16_t sx = x0<x1 ? 1 : -1;
    int16_t dy = y1 > y0 ? -(y1-y0): y1-y0;
    int16_t sy = y0<y1 ? 1 : -1;
    int16_t err = dx+dy;  /* error value e_xy */
    while (1)   /* loop */
    {
        GPU_DrawPixel(color, x0, y0);
        if (x0 == x1 && y0 == y1) 
            break;
        int16_t e2 = 2*err;
        if (e2 >= dy) /* e_xy+e_x > 0 */
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) /* e_xy+e_y < 0 */
        {
            err += dx;
            y0 += sy;
        }
    }
}

void GPU_render (){
    ili9341_write_data(_framebuffer, 320*240*2);
}

// DRAW SHAPESS

void GPU_DrawEmptySquare(uint16_t color, int16_t x, int16_t y, int16_t width, int16_t height)
{
    GPU_DrawLine(color, x, y, x+width, y);
    GPU_DrawLine(color, x, y+height, x+width, y+height);
    GPU_DrawLine(color, x, y, x, y+height);
    GPU_DrawLine(color, x+width, y, x+width, y+height);
}

void GPU_DrawFilledSquare(uint16_t color, int16_t x, int16_t y, int16_t width, int16_t height)
{
    for (int16_t i=0; i<width; i++)
    {
        for (int16_t j=0; j<height; j++)
        {
            GPU_DrawPixel(color, x+i, y+j);
        }    
    }
}

void GPU_DrawEmptyCircle(uint16_t color, int16_t x, int16_t y, int16_t radius)
{
    uint16_t xd = 0, yd = radius;
    int16_t d = 3 - 2 * radius;
    GPU_DrawPixel(color, x+xd, y+yd);
    GPU_DrawPixel(color, x-xd, y+yd);
    GPU_DrawPixel(color, x+xd, y-yd);
    GPU_DrawPixel(color, x-xd, y-yd);
    GPU_DrawPixel(color, x+yd, y+xd);
    GPU_DrawPixel(color, x-yd, y+xd);
    GPU_DrawPixel(color, x+yd, y-xd);
    GPU_DrawPixel(color, x-yd, y-xd);
    while (yd >= xd)
    {
        xd++;
        if (d > 0)
        {
            yd--;
            d = d + 4 * (xd - yd) + 10;
        }
        else
            d = d + 4 * xd + 6;
        GPU_DrawPixel(color, x+xd, y+yd);
        GPU_DrawPixel(color, x-xd, y+yd);
        GPU_DrawPixel(color, x+xd, y-yd);
        GPU_DrawPixel(color, x-xd, y-yd);
        GPU_DrawPixel(color, x+yd, y+xd);
        GPU_DrawPixel(color, x-yd, y+xd);
        GPU_DrawPixel(color, x+yd, y-xd);
        GPU_DrawPixel(color, x-yd, y-xd);
    }
}

void GPU_DrawFilledCircle(uint16_t color, int16_t x, int16_t y, int16_t radius)
{
    uint16_t xd = 0, yd = radius;
    int16_t d = 3 - 2 * radius;
    int16_t mxxd = x-xd > 0 ? x-xd : 0;
    int16_t myyd = y-yd > 0 ? y-yd : 0;
    int16_t mxyd = x-yd > 0 ? x-yd : 0;
    int16_t myxd = y-xd > 0 ? y-xd : 0;
    GPU_DrawLine(color, mxxd, myyd, x+xd, myyd);
    GPU_DrawLine(color, mxxd, y+yd, x+xd, y+yd);
    GPU_DrawLine(color, mxyd, myxd, x+yd, myxd);
    GPU_DrawLine(color, mxyd, y+xd, x+yd, y+xd);
    while (yd >= xd)
    {
        xd++;
        if (d > 0)
        {
            yd--;
            d = d + 4 * (xd - yd) + 10;
        }
        else
            d = d + 4 * xd + 6;
        
        mxxd = x-xd > 0 ? x-xd : 0;
        myyd = y-yd > 0 ? y-yd : 0;
        mxyd = x-yd > 0 ? x-yd : 0;
        myxd = y-xd > 0 ? y-xd : 0;
        GPU_DrawLine(color, x-xd, y-yd, x+xd, y-yd);
        GPU_DrawLine(color, x-xd, y+yd, x+xd, y+yd);
        GPU_DrawLine(color, x-yd, y-xd, x+yd, y-xd);
        GPU_DrawLine(color, x-yd, y+xd, x+yd, y+xd);
    }
}

// Draw Text for fixed size

void GPU_DrawLetter(uint16_t color, uint8_t  fontDescription, int16_t x, int16_t y, char letter)
{
    for (uint16_t i=0; i<12; i++)
    {
        uint8_t current_pic = font_data[(letter-32)][i];
        for (uint8_t j=0; j<8; j++)
        {
            if (current_pic & (1<<(8-j)))
                GPU_DrawPixel(color, x+j, y+i);
        }
    }
}

void GPU_DrawText(uint16_t color, uint8_t  fontDescription, int16_t x, int16_t y, const char * text, uint16_t len)
{
    for (uint16_t i=0; i<len; i++)
    {
        GPU_DrawLetter(color, fontDescription, x+i*8, y, text[i]);
    }
}


// printing larger text 

void GPU_DrawLetter_L(uint16_t color, int16_t x, int16_t y, char letter)

{
    
    const FontDescription * fontDescription = f_big;

    for (uint16_t i=0; i<fontDescription->height; i++)
    {
        uint16_t current_pic = fontDescription->fontArray[(letter-32)*fontDescription->height+i];
        for (uint8_t j=0; j<fontDescription->width; j++)
        {
            if (current_pic & (1<<(16-j)))
                GPU_DrawPixel(color, x+j, y+i);
        }
    }
}

void GPU_DrawText_L(uint16_t color, int16_t x, int16_t y, const char * text, uint16_t len)
{

        const FontDescription * fontDescription = f_big;
    for (uint16_t i=0; i<len; i++)
    {
        GPU_DrawLetter_L(color,  x+i*fontDescription->width, y, text[i]);
    }
}

void GPU_draw_icon(uint16_t color, int16_t x, int16_t y) {
    // assert depth == 0?
for (int i = 0; i < 24; i++){
    for(int j = 23; j>=0; j--){
             
                uint8_t pixel_data = icon_pack[j*24 + i];
                pixel_data = pixel_data%16;
                GPU_DrawPixel(pixel_data, x+i, y+j);

            }
        
    }
}

// GPU _ DRAW _ ICON multiple
void GPU_draw_icon_C(uint16_t color, mode0_icon_t icon,  int16_t x, int16_t y) {
    // assert depth == 0?
for (int i = 0; i < 24; i++){
    for(int j = 23; j>=0; j--){
             
                uint8_t * icon_array = icons[icon]; 
                uint8_t pixel_data = icon_array[j*24 + i];
                pixel_data = pixel_data%16;



                // cahnginh pixel code 

                GPU_DrawPixel(pixel_data, x+i, y+j);


            // if (x >= WIDTH|| y >= HEIGHT)
            //     {int z = 0 ;}
            // else {              uint16_t * pix = &_framebuffer[(WIDTH-x-1)*HEIGHT+y];
            //                     (*pix) = (uint16_t) pixel_data ;
            // }

            }
        
    }
}

void GPU_draw_wallpaper(uint16_t color, mode0_icon_t icon,  int16_t x, int16_t y) {
    // assert depth == 0?
for (int i = 0; i < 320; i++){
    for(int j = 176; j>=10; j--){
             
                uint8_t * icon_array = icons[icon]; 
                uint8_t pixel_data = icon_array[j*320 + i];
                if (pixel_data != 0 ){

                pixel_data = 13;
                //pixel_data%16;
                GPU_DrawPixel(pixel_data, x+i, y+j);
                }



                // cahnginh pixel code 




            // if (x >= WIDTH|| y >= HEIGHT)
            //     {int z = 0 ;}
            // else {              uint16_t * pix = &_framebuffer[(WIDTH-x-1)*HEIGHT+y];
            //                     (*pix) = (uint16_t) pixel_data ;
            // }

            }
        
    }
}

// GPU DRAW rgb 565 icon 

void GPU_DrawPixel_565( uint16_t color, uint16_t x, uint16_t y)
{
    if (x >= WIDTH|| y >= HEIGHT)
        return;
    uint16_t * pix = &_framebuffer[(WIDTH-x-1)*HEIGHT+y];
    (*pix) = (uint16_t) color;
}



void GPU_draw_icon_565( int16_t x, int16_t y) {
    // assert depth == 0?
for (int i = 0; i < 32; i++){
    for(int j = 31; j>=0; j--){
             
                uint8_t * icon_array = &back_2_map ; 
                uint16_t pixel_data_R = icon_array[j*64 + i*2 ];
                uint16_t pixel_data_gb = icon_array[j*64+ i*2 +1];
                uint16_t pixel = ((pixel_data_gb & 0x00ff) << 8) | (pixel_data_R & 0x00ff);

                // cahnginh pixel code 

                GPU_DrawPixel_565(pixel, x+i, y+j);

            // if (x >= WIDTH|| y >= HEIGHT)
            //     {int z = 0 ;}
            // else {              uint16_t * pix = &_framebuffer[(WIDTH-x-1)*HEIGHT+y];
            //                     (*pix) = (uint16_t) pixel_data ;
            // }
            }
        
    }
}



