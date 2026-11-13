#include <debug.h>  // for scr_printf
#include <unistd.h> // for sleep
#include <debug.h>
#include <libpad.h>

static int GalaPadBuffer[256] __attribute__((aligned(64)));
static int ret = -35;

int main(void) 
{
    //initialize the screen
    init_scr();
    sleep(1);

    //init libpad
    ret = padInit(0);
    scr_printf("padInit: %d", ret);

    ret = -35;
    ret = padPortOpen(0, 0, GalaPadBuffer);
    scr_printf("padPortOpen (!= 0: OK): %d", ret);

    sleep(10);

    //scr_printf("Hello, World!");
    //sleep(10);
    
    return 0;
}

