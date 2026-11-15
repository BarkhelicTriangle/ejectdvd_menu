#include <debug.h> 
#include <unistd.h>
#include <libpad.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <kernel.h>

static int GalaPadBuffer[256] __attribute__((aligned(64)));
static int ret = -35;

void wait_for_pad(int port, int slot) 
{
    int padState;
    while (1)
    {
        padState = padGetState(port, slot);
        if (padState == PAD_STATE_STABLE)
            break;

        sleep(1);
    }

    return;
}

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
    wait_for_pad(0, 0);    
    scr_printf("Ready!\n");

    scr_printf("No functionality implemented yet. START to exit");

    struct padButtonStatus buttons;
    u16 buttons_prevframe = 0xFFFF;
    u16 buttons_thisframe;
    while (1) 
    {
        //check button presses
        ret = padRead(0, 0, &buttons);
        if (ret == 0)
            scr_printf("padRead errored.\n");

        buttons_thisframe = 0xFFFF ^ buttons.btns;
            
        // Discern what buttons were actually pressed by comparing readings
        u16 pressed = buttons_thisframe & ~buttons_prevframe;

        // exit if START pressed
        if (pressed & PAD_START)
        {
            break;
        }

        buttons_prevframe = buttons_thisframe;

        RotateThreadReadyQueue(0);
    }

    padPortClose(0, 0);
    padEnd();
    SifLoadFileExit();

    return 0;
}