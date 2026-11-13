#include <debug.h>  // for scr_printf
#include <unistd.h> // for sleep
#include <debug.h>
#include <libpad.h>

int main(void) 
{
    //initialize the screen
    init_scr();
    sleep(1);


    int ret = -35;

    //init loadman and libpad
    ret = padInit(0);
    scr_printf("padInit: %d", ret);
    sleep(10);

    //scr_printf("Hello, World!");
    //sleep(10);
    
    return 0;
}

