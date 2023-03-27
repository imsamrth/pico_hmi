#include "pico/stdlib.h"
#include "mode0/mode0.h"
#include <unistd.h> // notice this! you need it!
#include "pico/time.h"

int main() {
    mode0_init();
    
    mode0_set_cursor(0, 0);
    mode0_color_t fg = MODE0_WHITE;
    mode0_color_t bg = MODE0_BLACK;

    char letter = 'S';
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    char title[] = "APPS";
    char * titleadd = &title ;
    char date[] = "27 Feb";
    char * dateadd = &date ;

    char samplenames [5][12] = {
"Devices     ",
"Notification",
"Files       ",
"Settings    ",
"Wallpaper   "};

    char samplename[] = "Files";
    char * sampleadd = &samplename ;

    // to print icon 
    
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 0, 320, 240);
    GPU_DrawFilledSquare(MODE0_DESERT, 0, 0, 320, 40);
    GPU_DrawFilledSquare(MODE0_WHITE, 0, 40, 320, 40);
    GPU_draw_icon_C(MODE0_ORANGE, MODE0_NOTI ,10, 8);
    GPU_draw_icon_C(MODE0_ORANGE, MODE0_BT , 250, 8);
    GPU_draw_icon_C(MODE0_ORANGE, MODE0_BATT ,290, 8);
    GPU_draw_icon_C(MODE0_ORANGE, MODE0_MORE ,120, 48);
    GPU_DrawText_L(MODE0_BLACK, 150, 47, titleadd ,4);
    GPU_DrawFilledSquare(MODE0_GRAY, 308, 80, 10, 160);   


    for (int i = 0; i < 4; i ++){

        GPU_DrawFilledSquare(MODE0_PALE_BLUE, 5, 83 + i*40, 40, 34);
        GPU_draw_icon_C(MODE0_ORANGE, i ,13, 88 + i*40);
        GPU_DrawFilledSquare(MODE0_GRAY, 45, 83 + i*40, 260, 34); 
        GPU_DrawText_L(MODE0_BLACK, 55, 87 +i*40 , &samplenames[i] ,10);
        GPU_DrawEmptySquare(MODE0_BLACK,5,83 + i*40, 300,34);
    }

    //GPU_DrawEmptySquare(MODE0_BLACK,10,120, 300,40);

    while(1){
        for ( int i = 0 ; i < 4 ; i++){

    GPU_DrawEmptySquare(MODE0_RED, 5, 83 + i*40, 300, 34); 
    GPU_DrawEmptySquare(MODE0_RED, 6, 84 + i*40, 298, 32); 
    GPU_DrawEmptySquare(MODE0_RED, 7, 84 + i*40, 296, 30); 
    GPU_DrawFilledSquare(MODE0_BLACK, 308, 80 + 40*i, 10, 40);  

    GPU_render();

    sleep_ms(1000);

    GPU_DrawEmptySquare(MODE0_WHITE, 5, 83 + i*40, 300, 34); 
    GPU_DrawEmptySquare(MODE0_WHITE, 6, 84 + i*40, 298, 32); 
    GPU_DrawEmptySquare(MODE0_WHITE, 7, 84 + i*40, 296, 30); 
        GPU_DrawFilledSquare(MODE0_GRAY, 308, 80, 10, 160);   
    }

    }

    // for (int i = 0 ; i < 4 ; i++){

    //     for(int j = 0 ; j < 5000; j++){
    //         j = j*2 ;
    //         j = j/2 ;
    //     }
    // GPU_DrawEmptySquare(MODE0_RED, 10, 83 + i*40, 300, 34); 
    // GPU_DrawEmptySquare(MODE0_RED, 11, 84 + i*40, 298, 32); 
    // GPU_DrawEmptySquare(MODE0_RED, 12, 84 + i*40, 296, 30); 
    // GPU_render();
    // }
    // GPU_draw_icon_C(MODE0_ORANGE, MODE0_MORE ,10, 48);


    // GPU_draw_wallpaper(MODE0_ORANGE, MODE_WEL, 0, 40);
    // GPU_DrawFilledSquare(MODE0_WHITE, 0, 200, 320, 40); 


    // GPU_DrawText_L(MODE0_BLACK, 120, 100, timeadd ,5);
    // GPU_DrawFilledSquare(MODE0_GRAY, 132, 130, 48, 12);
    // GPU_DrawText(MODE0_RED, 1, 132, 130, dateadd ,6);




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
