#include "pico/stdlib.h"
#include "mode0/mode0.h"


int main() {
    mode0_init();
    
    mode0_set_cursor(0, 0);
    mode0_color_t fg = MODE0_WHITE;
    mode0_color_t bg = MODE0_BLACK;
        const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // to print pixel
    

    GPU_DrawLine(MODE0_WHITE, 120, 0,  120, 230);
        GPU_DrawLine(MODE0_WHITE, 121, 0,  121, 230);
            GPU_DrawLine(MODE0_WHITE, 122, 0,  122, 230);
                GPU_DrawLine(MODE0_WHITE, 123, 0,  123, 230);
                    GPU_DrawLine(MODE0_WHITE, 124, 0,  124, 230);
    //GPU_DrawPixel(MODE0_GREEN, 120, 120);
    GPU_render();


    // ENd here 

    while (0) {
        mode0_print("Retro Computer (c) 2021, Shawn Hyam");
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
