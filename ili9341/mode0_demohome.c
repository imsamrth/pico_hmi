#include "pico/stdlib.h"
#include "mode0/mode0.h"

int main() {
    mode0_init();
    
    mode0_set_cursor(0, 0);
    mode0_color_t fg = MODE0_WHITE;
    mode0_color_t bg = MODE0_BLACK;

    char letter = 'S';
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    char time[] = "03:59";
    char * timeadd = &time ;
    char date[] = "27 Feb";
    char * dateadd = &date ;


    // to print icon 
    

    GPU_DrawFilledSquare(MODE0_WHITE, 0, 0, 320, 40);
    GPU_DrawFilledSquare(MODE0_GRAY, 0, 40, 320, 160);
   

    GPU_draw_icon_C(MODE0_ORANGE, MODE0_NOTI ,10, 8);
    GPU_draw_icon_C(MODE0_ORANGE, MODE0_BT , 250, 8);
    GPU_draw_icon_C(MODE0_ORANGE, MODE0_BATT ,290, 8);


    GPU_draw_wallpaper(MODE0_ORANGE, MODE_WEL, 0, 40);
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 200, 320, 40); 
        GPU_draw_icon_C(MODE0_ORANGE, MODE0_MORE ,148, 208);

    GPU_DrawText_L(MODE0_BLACK, 120, 100, timeadd ,5);
    GPU_DrawFilledSquare(MODE0_GRAY, 132, 130, 48, 12);
    GPU_DrawText(MODE0_RED, 1, 132, 130, dateadd ,6);




    // mode0_draw_icon() ;

    // GPU_DrawLine(MODE0_GREEN, 120, 0,  120, 230);
    // GPU_DrawLine(MODE0_GREEN, 121, 0,  121, 230);
    // GPU_DrawLine(MODE0_GREEN, 122, 0,  122, 230);
    // GPU_DrawLine(MODE0_GREEN, 123, 0,  123, 230);
    // GPU_DrawLine(MODE0_GREEN, 124, 0,  124, 230);

    // GPU_DrawEmptyCircle(MODE0_BLUE, 0, 0, 20);

    // GPU_DrawLetter(MODE0_ORANGE, 8 , 30, 30, letter);
    // //GPU_DrawPixel(MODE0_GREEN, 120, 120);

    // GPU_DrawFilledSquare(MODE0_WHITE, 50, 50, 100, 100);

    // GPU_draw_icon(MODE0_ORANGE, 50, 50);

    // GPU_draw_icon_C(MODE0_ORANGE, MODE0_NOTI ,80, 80);
    GPU_render();


    // ENd here 

    while (0) {
        mode0_print("Retro Computer (c) 2021, Shawn Hyam\n");
        sleep_ms(500);
        fg = (fg+1) % 16;
        if (fg == 0) {
            bg = (bg+1) % 16;
            mode0_set_background(bg);
        }
        mode0_set_foreground(fg);

        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);

    }
}
