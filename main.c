#include <debug.h>  // for scr_printf
#include <unistd.h> // for sleep
#include <debug.h>
#include <libpad.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <iopcontrol.h>

static int GalaPadBuffer[256] __attribute__((aligned(64)));
static int ret = -35;

int main(void) 
{
    // Reset IOP to ensure a clean slate
    while(!SifIopReset("", 0));
    while(!SifIopSync());
    
    sceSifInitRpc(0);

   //initialize the screen
    init_scr();
    sleep(1);

    //load padman
    SifLoadFileInit();
    ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
    ret = SifLoadModule("rom0:PADMAN", 0, NULL);  // manages controllers

    //init libpad
    ret = padInit(0);
    scr_printf("padInit: %d\n", ret);

    //open the controller
    ret = -35;
    ret = padPortOpen(0, 0, GalaPadBuffer);
    scr_printf("padPortOpen (!= 0: OK): %d\n", ret);

    //wait for controller to be ready...
    scr_printf("Waiting for controller to be ready... ");
    int padState;
    while (1)
    {
        padState = padGetState(0, 0);
        if (padState == PAD_STATE_STABLE)
            break;

        sleep(1);
    }
    scr_printf("Ready!\n");

    sleep(5);
    
    return 0;
}

