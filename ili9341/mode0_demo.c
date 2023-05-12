#include "pico/stdlib.h"
#include "include/mode0/mode0.h"
#include <unistd.h> // notice this! you need it!
#include "pico/time.h"
#include <stdio.h>
#include <stdlib.h>
#include "hardware/spi.h"
#include "gameshared.h"
#include "interface.h"
#include "mainloop.h"

// Main object developer for RPP object
int main()
{
    //configuring the LCD using init function
    mode0_init();
    mode0_set_cursor(0, 0);

    // setup key hearing and check its not null
    IF_Setup();

    // call homepage and run main loop
    SetupCallbacks(&HM_Callbacks);
    DoMainLoop();

}
