#include <debug.h> 
#include <unistd.h>
#include <libpad.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <kernel.h>
#include <libcdvd.h>

#define NDEBUG

int padBuffer[256] __attribute__((aligned(64)));
u8 ret;

void wait_for_pad(int port, int slot) 
{
    u8 padState;
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
    // Reset IOP
    while(SifIopReset("", 0) != 1);
    while(SifIopSync() != 1);
    
    sceSifInitRpc(0);

   //initialize the screen
    init_scr();
    sleep(1);

    //load necessary modules
    ret = SifLoadFileInit();
#ifndef NDEBUG
    scr_printf("SifLoadFileInit (!= 0: OK): %d\n", ret);
#endif
    ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);   // needed for controllers
#ifndef NDEBUG
    scr_printf("load SI02MAN: %d\n", ret);
#endif
    ret = SifLoadModule("rom0:PADMAN", 0, NULL);    // needed for controllers
#ifndef NDEBUG
    scr_printf("load PADMAN: %d\n", ret);
#endif
    ret = SifLoadModule("rom0:CDVDMAN", 0, NULL);   // needed for the CD/DVD drive
#ifndef NDEBUG
    scr_printf("load CDVDMAN: %d\n", ret);
#endif

    //init libcdvd
    ret = sceCdInit(SCECdINIT);
#ifndef NDEBUG
    scr_printf("sceCdInit (!= 0: OK): %d\n", ret);
#endif

    //init libpad
    ret = padInit(0);
#ifndef NDEBUG
    scr_printf("padInit: %d\n", ret);
#endif

    //open the controller
    ret = padPortOpen(0, 0, padBuffer);
#ifndef NDEBUG
    scr_printf("padPortOpen (!= 0: OK): %d\n", ret);
#endif

    //wait for controller to be ready...
    scr_printf("Waiting for controller to be ready... ");
    wait_for_pad(0, 0);    
    scr_printf("Ready!\n");

    scr_printf("CROSS = Open DVD Drive\nCIRCLE = Close DVD DRIVE\nSTART = Exit to browser\n");

    struct padButtonStatus buttons;
    u16 buttons_prevframe = 0xFFFF;
    u16 buttons_thisframe;
    while (1) 
    {
        //check button presses
        ret = padRead(0, 0, &buttons);
        if (ret == 0)
        {
            scr_printf("Can't read controller! Waiting for controller... ");
            wait_for_pad(0, 0);
            scr_printf("Reconnected.\n");
        }

        buttons_thisframe = 0xFFFF ^ buttons.btns;
            
        // Discern what buttons were actually pressed by comparing readings
        u16 pressed = buttons_thisframe & ~buttons_prevframe;

        // --- Actual functionality logic begins here. ---
        if (pressed & PAD_START)
            break;

        if (pressed & PAD_CROSS) 
        {
            //check if the tray is already open to prevent errors
            if (sceCdStatus() == SCECdStatShellOpen)
            {
                scr_printf("Tray is already open!\n");
            }
            else
            {
                scr_printf("Opening...\n");
                ret = sceCdTrayReq(SCECdTrayOpen, NULL);
                if (ret != 1)
                    scr_printf("Open tray failed: %d\n", ret);
            }
        }
    
        if (pressed & PAD_CIRCLE)
        {
            //check if the tray is already closed to prevent errors
            if (sceCdStatus() != SCECdStatShellOpen)
            {
                scr_printf("Tray is already closed!\n");
            }
            else
            {
                scr_printf("Closing...\n");
                ret = sceCdTrayReq(SCECdTrayClose, NULL);
                if (ret != 1)
                    scr_printf("Close tray failed: %d\n", ret);
            }
        }
        // ------------------------------------------------

        buttons_prevframe = buttons_thisframe;
        RotateThreadReadyQueue(0);
    }

    //de-initialize the stuff we used
    sceCdInit(SCECdEXIT);
    padPortClose(0, 0);
    padEnd();
    SifLoadFileExit();

    return 0;
}