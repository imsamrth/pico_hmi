#include "pico/stdlib.h"
#include <string.h>
#include "hardware/spi.h"
#include "include/ili9341/ili9341.h"
#include "include/mode0/mode0.h"
#include "include/font/font.h"
#include "include/icon/icon.h"
#include <stdint.h>

// intializinf screen/ display configuations 
#define WIDTH 320
#define HEIGHT 240

// characters space height and width
#define TEXT_HEIGHT 24
#define TEXT_WIDTH 53

// load color in ILI9341 with bytes swapped
#define SWAP_BYTES(color) ((uint16_t)(color >> 8) | (uint16_t)(color << 8))

// initilize array that will contain the color at every pixel
static uint16_t _framebuffer[WIDTH * HEIGHT] = {0}; // GPU VRAM (16bits per color)
static uint16_t _gpuTexture[TEXTURE_SIZE*TEXTURE_SIZE] = {0};

// other varaibles 
static mode0_color_t screen_bg_color = MODE0_BLACK;
static mode0_color_t screen_fg_color = MODE0_WHITE; // TODO need to store a color per cell
static int cursor_x = 0;
static int cursor_y = 0;


static uint8_t screen[TEXT_HEIGHT * TEXT_WIDTH] = {0};
static uint8_t colors[TEXT_HEIGHT * TEXT_WIDTH] = {0};
static uint8_t show_cursor = 0;
static uint8_t *files32 = &files_32;
static uint8_t *files64 = &files64;

static const FontDescription *f_big = &fontDescription[3];
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
    SWAP_BYTES(0xB6FD)};

static uint8_t *icons[5] = {
    &notification_map,
    &battery_map,
    &more_map,
    &bluetooth_map,
    &wel_map};

void mode0_clear(mode0_color_t color)
{
    mode0_begin();
    int size = TEXT_WIDTH * TEXT_HEIGHT;
    memset(screen, 0, size);
    memset(colors, color, size);
    mode0_set_cursor(0, 0);
    mode0_end();
}

void mode0_set_foreground(mode0_color_t color)
{
    mode0_begin();
    screen_fg_color = color;
    mode0_end();
}

void mode0_set_background(mode0_color_t color)
{
    mode0_begin();
    screen_bg_color = color;
    mode0_end();
}

void mode0_set_cursor(uint8_t x, uint8_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void mode0_show_cursor()
{
    mode0_begin();
    show_cursor = 1;
    mode0_end();
}

void mode0_hide_cursor()
{
    mode0_begin();
    show_cursor = 0;
    mode0_end();
}

uint8_t mode0_get_cursor_x()
{
    return cursor_x;
}

uint8_t mode0_get_cursor_y()
{
    return cursor_y;
}

inline void mode0_begin()
{
    depth++;
}

inline void mode0_end()
{
    if (--depth == 0)
    {
        mode0_draw_screen();
    }
}

void mode0_draw_region(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    // TODO
    mode0_draw_screen();
}

void mode0_draw_screen()
{
    // assert depth == 0?
    depth = 0;

    // setup to draw the whole screen

    // column address set
    ili9341_set_command(ILI9341_CASET);
    ili9341_command_param(0x00);
    ili9341_command_param(0x00); // start column
    ili9341_command_param(0x00);
    ili9341_command_param(0xef); // end column -> 239

    // page address set
    ili9341_set_command(ILI9341_PASET);
    ili9341_command_param(0x00);
    ili9341_command_param(0x00); // start page
    ili9341_command_param(0x01);
    ili9341_command_param(0x3f); // end page -> 319

    // start writing
    ili9341_set_command(ILI9341_RAMWR);

    uint16_t buffer[6 * 240]; // 'amount' pixels wide, 240 pixels tall

    int screen_idx = 0;
    for (int x = 0; x < TEXT_WIDTH; x++)
    {
        // create one column of screen information

        uint16_t *buffer_idx = buffer;

        for (int bit = 0; bit < 6; bit++)
        {
            uint8_t mask = 64 >> bit;
            for (int y = TEXT_HEIGHT - 1; y >= 0; y--)
            {
                uint8_t character = screen[y * 53 + x];
                uint16_t fg_color = palette[colors[y * 53 + x] >> 4];
                uint16_t bg_color = palette[colors[y * 53 + x] & 0xf];

                if (show_cursor && (cursor_x == x) && (cursor_y == y))
                {
                    bg_color = MODE0_GREEN;
                }

                const uint8_t *pixel_data = font_data[character];

                // draw the character into the buffer
                for (int j = 10; j >= 1; j--)
                {
                    //*buffer_idx++ = (pixel_data[j] & mask) ? fg_color : bg_color;
                    *buffer_idx++ = bg_color;
                    //(pixel_data[j] & mask) ?  : bg_color;
                }
            }
        }

        // now send the slice
        ili9341_write_data(buffer, 6 * 240 * 2);
    }

    uint16_t extra_buffer[2 * 240] = {0};
    ili9341_write_data(extra_buffer, 2 * 240 * 2);
}

void mode0_init()
{
    stdio_init_all();

    ili9341_init();
}



// gpu clear
void GPU_ClearFramebuffers()
{
    memset(_framebuffer, 0, sizeof(_framebuffer));
}

void GPU_ClearTexture()
{
    memset(_gpuTexture, 0, sizeof(_gpuTexture));
}

//texture  is like frame, but to cover display
// and not changing the actual display 
mode0_color_t Texture_GetPixel(uint8_t x, uint8_t y)
{
	return _gpuTexture[y*TEXTURE_SIZE+x];
}

void Texture_DrawLetter(mode0_color_t color, const FontDescription * fontDescription, int16_t x, int16_t y, char letter)
{
    for (uint16_t i=0; i<fontDescription->height; i++)
    {
        uint16_t current_pic = fontDescription->fontArray[(letter-32)*fontDescription->height+i];
        for (uint8_t j=0; j<fontDescription->width; j++)
        {
            if (current_pic & (1<<(16-j)))
                Texture_DrawPixel(color, x+j, y+i);
        }
    }
}


void GPU_DrawRotatedTexture(int16_t x, int16_t y, float factor, float angle)
{
    if (factor == 0)
        return;
    const float s = sin(angle);
    const float c = cos(angle);
    const uint16_t tex_size = TEXTURE_SIZE * factor;
    const int16_t d = tex_size / 2;
    float cx = x - d * (c + s);
    float cy = y - d * (c - s);
    float step = 1.0/factor;
    float ix = 0;
    float iy = 0;
    for (uint16_t i = 0; i < tex_size; i++)
    {
        float rx = cx;
        float ry = cy;
        iy = 0;
        for (uint16_t j = 0; j < tex_size; j++)
        {
            mode0_color_t pixel = Texture_GetPixel(ix, iy);
            GPU_DrawPixel(pixel, rx, ry);
            rx += s;
            ry += c;
            iy += step;
        }
        cx += c;
        cy -= s;
        ix += step;
    }
}

// DRAW GPU VECTORS

void GPU_DrawPixel(uint16_t color, uint16_t x, uint16_t y)
{
    if (x >= WIDTH || y >= HEIGHT)
        return;
    uint16_t *pix = &_framebuffer[(WIDTH - x - 1) * HEIGHT + y];
    (*pix) = (uint16_t)palette[color];
}

void GPU_DrawLine(uint16_t color, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    // Bresenhams line algorithm
    x0 = (x0 >= WIDTH) ? WIDTH - 1 : x0;
    x1 = (x1 >= WIDTH) ? WIDTH - 1 : x1;
    y0 = (y0 >= HEIGHT) ? HEIGHT - 1 : y0;
    y1 = (y1 >= HEIGHT) ? HEIGHT - 1 : y1;
    int16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t dy = y1 > y0 ? -(y1 - y0) : y1 - y0;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy; /* error value e_xy */
    while (1)              /* loop */
    {
        GPU_DrawPixel(color, x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        int16_t e2 = 2 * err;
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

void GPU_render()
{
    ili9341_write_data(_framebuffer, 320 * 240 * 2);
}

// DRAW SHAPESS

void GPU_DrawEmptySquare(uint16_t color, int16_t x, int16_t y, int16_t width, int16_t height)
{
    GPU_DrawLine(color, x, y, x + width, y);
    GPU_DrawLine(color, x, y + height, x + width, y + height);
    GPU_DrawLine(color, x, y, x, y + height);
    GPU_DrawLine(color, x + width, y, x + width, y + height);
}

void GPU_DrawFilledSquare(uint16_t color, int16_t x, int16_t y, int16_t width, int16_t height)
{
    for (int16_t i = 0; i < width; i++)
    {
        for (int16_t j = 0; j < height; j++)
        {
            GPU_DrawPixel(color, x + i, y + j);
        }
    }
}

void GPU_DrawEmptyCircle(uint16_t color, int16_t x, int16_t y, int16_t radius)
{
    uint16_t xd = 0, yd = radius;
    int16_t d = 3 - 2 * radius;
    GPU_DrawPixel(color, x + xd, y + yd);
    GPU_DrawPixel(color, x - xd, y + yd);
    GPU_DrawPixel(color, x + xd, y - yd);
    GPU_DrawPixel(color, x - xd, y - yd);
    GPU_DrawPixel(color, x + yd, y + xd);
    GPU_DrawPixel(color, x - yd, y + xd);
    GPU_DrawPixel(color, x + yd, y - xd);
    GPU_DrawPixel(color, x - yd, y - xd);
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
        GPU_DrawPixel(color, x + xd, y + yd);
        GPU_DrawPixel(color, x - xd, y + yd);
        GPU_DrawPixel(color, x + xd, y - yd);
        GPU_DrawPixel(color, x - xd, y - yd);
        GPU_DrawPixel(color, x + yd, y + xd);
        GPU_DrawPixel(color, x - yd, y + xd);
        GPU_DrawPixel(color, x + yd, y - xd);
        GPU_DrawPixel(color, x - yd, y - xd);
    }
}

void GPU_DrawFilledCircle(uint16_t color, int16_t x, int16_t y, int16_t radius)
{
    uint16_t xd = 0, yd = radius;
    int16_t d = 3 - 2 * radius;
    int16_t mxxd = x - xd > 0 ? x - xd : 0;
    int16_t myyd = y - yd > 0 ? y - yd : 0;
    int16_t mxyd = x - yd > 0 ? x - yd : 0;
    int16_t myxd = y - xd > 0 ? y - xd : 0;
    GPU_DrawLine(color, mxxd, myyd, x + xd, myyd);
    GPU_DrawLine(color, mxxd, y + yd, x + xd, y + yd);
    GPU_DrawLine(color, mxyd, myxd, x + yd, myxd);
    GPU_DrawLine(color, mxyd, y + xd, x + yd, y + xd);
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

        mxxd = x - xd > 0 ? x - xd : 0;
        myyd = y - yd > 0 ? y - yd : 0;
        mxyd = x - yd > 0 ? x - yd : 0;
        myxd = y - xd > 0 ? y - xd : 0;
        GPU_DrawLine(color, x - xd, y - yd, x + xd, y - yd);
        GPU_DrawLine(color, x - xd, y + yd, x + xd, y + yd);
        GPU_DrawLine(color, x - yd, y - xd, x + yd, y - xd);
        GPU_DrawLine(color, x - yd, y + xd, x + yd, y + xd);
    }
}

// Draw Text for fixed size

void GPU_DrawLetter(uint16_t color, uint8_t fontDescription, int16_t x, int16_t y, char letter)
{
    for (uint16_t i = 0; i < 12; i++)
    {
        uint8_t current_pic = font_data[(letter - 32)][i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (current_pic & (1 << (8 - j)))
                GPU_DrawPixel(color, x + j, y + i);
        }
    }
}

void GPU_DrawText(uint16_t color, uint8_t fontDescription, int16_t x, int16_t y, const char *text, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        GPU_DrawLetter(color, fontDescription, x + i * 8, y, text[i]);
    }
}

// GPU DRAW TEXT TEST of small fix size

void FGPU_DrawLetter(uint16_t color, const FontDescription *fontDescription, int16_t x, int16_t y, char letter)
{
    for (uint16_t i = 0; i < fontDescription->height; i++)
    {
        uint16_t current_pic = fontDescription->fontArray[(letter - 32) * fontDescription->height + i];
        for (uint8_t j = 0; j < fontDescription->width; j++)
        {
            if (current_pic & (1 << (16 - j)))
                GPU_DrawPixel(color, x + j, y + i);
        }
    }
}

void FGPU_DrawText(uint16_t color, const FontDescription *fontDescription, int16_t x, int16_t y, const char *text, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        FGPU_DrawLetter(color, fontDescription, x + i * fontDescription->width, y, text[i]);
    }
}

// printing larger text with argumnet that takes input of size

void GPU_DrawLetter_L(uint16_t color, int16_t x, int16_t y, char letter)

{

    const FontDescription *fontDescription = f_big;

    for (uint16_t i = 0; i < fontDescription->height; i++)
    {
        uint16_t current_pic = fontDescription->fontArray[(letter - 32) * fontDescription->height + i];
        for (uint8_t j = 0; j < fontDescription->width; j++)
        {
            if (current_pic & (1 << (16 - j)))
                GPU_DrawPixel(color, x + j, y + i);
        }
    }
}

void GPU_DrawText_L(uint16_t color, int16_t x, int16_t y, const char *text, uint16_t len)
{

    const FontDescription *fontDescription = f_big;
    for (uint16_t i = 0; i < len; i++)
    {
        GPU_DrawLetter_L(color, x + i * fontDescription->width, y, text[i]);
    }
}

void GPU_draw_icon(uint16_t color, int16_t x, int16_t y)
{
    // assert depth == 0?
    for (int i = 0; i < 24; i++)
    {
        for (int j = 23; j >= 0; j--)
        {

            uint8_t pixel_data = icon_pack[j * 24 + i];
            pixel_data = pixel_data % 16;
            GPU_DrawPixel(pixel_data, x + i, y + j);
        }
    }
}

// GPU _ DRAW _ ICON multiple of fixed size 24* 24
void GPU_draw_icon_C(uint16_t color, mode0_icon_t icon, int16_t x, int16_t y)
{
    // assert depth == 0?
    for (int i = 0; i < 24; i++)
    {
        for (int j = 23; j >= 0; j--)
        {

            uint8_t *icon_array = icons[icon];
            uint8_t pixel_data = icon_array[j * 24 + i];
            pixel_data = pixel_data % 16;

            // cahnginh pixel code

            GPU_DrawPixel(pixel_data, x + i, y + j);

            // if (x >= WIDTH|| y >= HEIGHT)
            //     {int z = 0 ;}
            // else {              uint16_t * pix = &_framebuffer[(WIDTH-x-1)*HEIGHT+y];
            //                     (*pix) = (uint16_t) pixel_data ;
            // }
        }
    }
}

void GPU_draw_wallpaper(uint16_t color, mode0_icon_t icon, int16_t x, int16_t y)
{
    // assert depth == 0?
    for (int i = 0; i < 320; i++)
    {
        for (int j = 176; j >= 10; j--)
        {

            uint8_t *icon_array = icons[icon];
            uint8_t pixel_data = icon_array[j * 320 + i];
            if (pixel_data != 0)
            {

                pixel_data = 13;
                // pixel_data%16;
                GPU_DrawPixel(pixel_data, x + i, y + j);
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

// GPU DRAW rgb 565 icon, this takes size of icon as input and colors are original

void GPU_DrawPixel_565(uint16_t color, uint16_t x, uint16_t y)
{
    if (x >= WIDTH || y >= HEIGHT)
        return;
    uint16_t *pix = &_framebuffer[(WIDTH - x - 1) * HEIGHT + y];
    (*pix) = (uint16_t)color;
}

void GPU_draw_icon_565(int16_t x, int16_t y, int16_t width, int16_t height, mode0_icon_t icon){
    // assert depth == 0?
    for (int i = 0; i < width; i++)
    {
        for (int j = height; j >= 0; j--)
        {

            uint8_t *icon_array = IconPack[icon];
            uint16_t pixel_data_R = icon_array[2 * j * width + 2 * i];
            uint16_t pixel_data_gb = icon_array[2 * j * width + 2 * i + 1];
            uint16_t pixel = ((pixel_data_gb & 0x00ff) << 8) | (pixel_data_R & 0x00ff);
            if (pixel != 0x0000)
                GPU_DrawPixel_565(pixel, x + i, y + j);
            // cahnginh pixel code

            // if (x >= WIDTH|| y >= HEIGHT)
            //     {int z = 0 ;}
            // else {              uint16_t * pix = &_framebuffer[(WIDTH-x-1)*HEIGHT+y];
            //                     (*pix) = (uint16_t) pixel_data ;
            // }
        }
    }
}
