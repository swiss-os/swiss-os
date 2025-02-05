
/******************************************************************************
 *                       OS-3o3 operating system
 *
 *                       Main kernel entry point
 *
 *            20 June MMXXIV PUBLIC DOMAIN by Jean-Marc Lienher
 *
 *        The authors disclaim copyright and patents to this software.
 *
 *****************************************************************************/

/*
 Size of io.sys is 384kB max due to limitation in io16.S

 */

#include "../include/klib.h"
#include "../include/rtos.h"


void knl__thread_main(void *arg)
{
    while (1)
    {
        k__printf(".");
        rtos__thread_sleep(100);
    }
}

void knl__thread_usb(void *arg)
{
    xhci__init();
    while (1)
    {
        k__printf("thread USB\n");
        rtos__thread_sleep(1000);
    }
}


int main(void)
{
    k__init();
    k__printf("\r\nStarting Swiss Operating System...");

    mmu__init();
    pci__init();

    k__printf(" @ 0x%x : 0x%x > 0x%x\r\n", k__fb, MEM_PROG_END, MEM_HEAP_SIZE);

    rtos__init((os_uint32 *)MEM_HEAP_START, MEM_HEAP_SIZE);

    rtos__thread_create("Main", knl__thread_main, NULL, 100, 4000);
    rtos__thread_create("USB", knl__thread_usb, NULL, 100, 40000);

    rtos__start();
    k__printf("End of Swiss OS...\n");
    while (1)
    {
        k__printf(".");
    }
    return 0;
}
