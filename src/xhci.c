/*
            MMXXIV October 11 PUBLIC DOMAIN by JML

     The authors and contributors disclaim copyright, patents
           and all related rights to this software.

 Anyone is free to copy, modify, publish, use, compile, sell, or
 distribute this software, either in source code form or as a
 compiled binary, for any purpose, commercial or non-commercial,
 and by any means.

 The authors waive all rights to patents, both currently owned
 by the authors or acquired in the future, that are necessarily
 infringed by this software, relating to make, have made, repair,
 use, sell, import, transfer, distribute or configure hardware
 or software in finished or intermediate form, whether by run,
 manufacture, assembly, testing, compiling, processing, loading
 or applying this software or otherwise.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT OF ANY PATENT, COPYRIGHT, TRADE SECRET OR OTHER
 PROPRIETARY RIGHT.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
 ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

/*
https://www.amazon.com/USB-Universal-Serial-Bus-8/dp/1717425364
http://www.intel.com/technology/usb/spec.htm
https://www.usb.org/document-library/usb-32-revision-11-june-2022
https://www.usb.org/document-library/usb-20-specification
https://github.com/fysnet/FYSOS/blob/master/main/usb/utils/gdevdesc/gd_xhci.h
https://github.com/fysnet/FYSOS/blob/master/main/usb/utils/gdevdesc/gd_xhci.c
https://github.com/fysnet/FYSOS/blob/master/main/usb/utils/include/xhci.h
https://cgit.freebsd.org/src/tree/sys/dev/usb/controller/xhci_pci.c
https://github.com/coreboot/seabios/blob/master/src/hw/usb-xhci.c
https://github.com/rsta2/circle/blob/master/lib/usb/xhcidevice.cpp
https://github.com/torvalds/linux/blob/master/drivers/usb/host/xhci.c
https://github.com/openbsd/src/blob/master/sys/dev/usb/xhci.c
https://gitlab.com/h0bby1/micro-kernel/-/blob/main/kernel/usb_bus/usb_bus.c
https://github.com/ChaiSoft/ChaiOS/blob/master/Chaikrnl/xhci.cpp
https://github.com/cherry-embedded/CherryUSB/blob/master/port/xhci/phytium/freertos/usb_glue_phytium.c
https://github.com/AdeRegt/SanderOSUSB/blob/master/kernel/dev/xhci.c
https://github.com/doug65536/dgos/blob/master/kernel/device/usb_xhci/usb_xhci.cc
https://github.com/SkyRzn/xhci/blob/master/xhci-pci.c
https://github.com/enukane/nbsd-xhci/blob/master/xhci.c
https://github.com/haiku/haiku/blob/master/src/add-ons/kernel/busses/usb/xhci.cpp
https://github.com/lixf/p9xhci/blob/master/usbxhci.c
https://github.com/mazino3/upanix-os/blob/master/drivers/usb/xhci/XHCIController.cpp
https://github.com/tianocore/edk2/blob/master/MdeModulePkg/Bus/Pci/XhciDxe/Xhci.c
https://github.com/hathach/tinyusb/blob/master/src/host/usbh.c
https://github.com/qemu/qemu/blob/master/hw/usb/hcd-xhci.c
https://github.com/bochs-emu/Bochs/blob/REL_2_8_FINAL/bochs/iodev/usb/usb_xhci.cc
https://github.com/abcamus/Designware-UHD3
https://github.com/LemonOSProject/LemonOS/blob/master/Kernel/src/USB/XHCI.cpp
https://github.com/reactos/reactos/tree/GSoC_2017/usbxhci/drivers/usb/usbxhci
https://gitlab.riscosopen.org/RiscOS/Sources/HWSupport/USB/Controllers/XHCIDriver/-/blob/master/c/xhci
https://github.com/zhonghaihping/tiny_xHCI/tree/master/kernel/usb/xhci
https://github.com/ipxe/ipxe/blob/master/src/drivers/usb/xhci.c
https://github.com/liva/xhci_uio
https://github.com/HelenOS/helenos/blob/master/uspace/drv/bus/usb/xhci/hc.c
https://github.com/xen-project/xen/blob/master/xen/drivers/char/xhci-dbc.c
https://github.com/copslock/ti_cpri/blob/master/packages/ti/drv/usb/src/xhci/usb_xhci_hcd.c
https://github.com/sysidos/fuchsia/blob/master/src/devices/usb/drivers/xhci/xhci.cc
https://sourceforge.net/p/prettyos/code/HEAD/tree/trunk/Source/kernel/usb/xhci.c
https://gitlab.com/nakst/essence/-/blob/master/drivers/xhci.cpp?ref_type=heads
    
*/

#include "../include/xhci.h"

#include "../include/usb.h"

#if 0
struct xhci_port
{
    os_uint8 is_usb3;
    os_uint8 is_high_speed_only;
    os_uint8 is_active;
    os_uint8 has_pair;
    os_uint8 other_port_num;
    os_uint8 offset;
};

struct xhci
{
    os_uint32 bus;
    os_uint32 slot;
    os_uint32 function;
    os_uint32 irq;
    os_uint32 base0;
    os_uint32 base1;
    os_uint32 size;
    os_uint32 b;

    void *heap;
    os_uint32 heap_size;
    os_uint32 heap_current;
    struct xhci_trb trb;

    struct xhci_port ports[XHCI_MAX_PORT];

    os_uint32 hccparams1, hccparams2, hcsparams1, hcsparams2, rts_offset, db_offset;
    os_uint32 context_size, page_size, op_base_off;
    os_uint32 dcbaap_start, max_slots;
    os_uint32 cmnd_ring_addr, cmnd_trb_addr, cmnd_trb_cycle;
    os_uint32 cur_event_ring_cycle;
    os_uint32 cur_event_ring_addr;
    os_uint32 no_deconfigure;
};

void xhci__event(struct xhci *ctrl);

void xhci__caps_write32(struct xhci *ctrl, os_uint32 offset, os_uint32 val)
{
    k__write32(ctrl->b + offset, val);
}

void xhci__caps_write64(struct xhci *ctrl, os_uint32 offset, os_uint32 low, os_uint32 high)
{
    k__write32(ctrl->b + offset, low);
    if (ctrl->hccparams1 & 1)
    {
        k__write32(ctrl->b + offset + 4, high);
    }
}

void xhci__oper_write32(struct xhci *ctrl, os_uint32 offset, os_uint32 val)
{
    xhci__caps_write32(ctrl, offset + ctrl->op_base_off, val);
}

void xhci__oper_write64(struct xhci *ctrl, os_uint32 offset, os_uint32 low, os_uint32 high)
{
    xhci__caps_write64(ctrl, offset + ctrl->op_base_off, low, high);
}

os_uint32 xhci__caps_read32(struct xhci *ctrl, os_uint32 offset)
{
    return k__read32(ctrl->b + offset);
}

os_uint32 xhci__caps_read64(struct xhci *ctrl, os_uint32 offset, os_uint32 *high)
{
    *high = 0;
    if (ctrl->hccparams1 & 1)
    {
        *high = k__read32(ctrl->b + offset + 4);
    }
    return k__read32(ctrl->b + offset);
}

os_uint32 xhci__oper_read32(struct xhci *ctrl, os_uint32 offset)
{
    return xhci__caps_read32(ctrl, offset + ctrl->op_base_off);
}

void xhci__phy_write32(struct xhci *ctrl, os_uint32 offset, os_uint32 val)
{
    os_uint32 *mem = (void *)offset;
    *mem = val;
}

os_uint32 xhci__phy_read32(struct xhci *ctrl, os_uint32 offset)
{
    os_uint32 *mem = (void *)offset;
    return *mem;
}

void xhci__phy_write64(struct xhci *ctrl, os_uint32 offset, os_uint32 low, os_uint32 high)
{
    os_uint32 *mem = (void *)offset;
    mem[0] = low;
    if (ctrl->hccparams1 & 1)
    {
        mem[1] = high;
    }
}

void xhci__runt_write32(struct xhci *ctrl, os_uint32 offset, os_uint32 val)
{
    os_uint32 *mem = (void *)ctrl->b;
    mem[(ctrl->rts_offset + offset) >> 2] = val;
}

os_uint32 xhci__runt_read32(struct xhci *ctrl, os_uint32 offset)
{
    os_uint32 *mem = (void *)ctrl->b;
    return mem[(ctrl->rts_offset + offset) >> 2];
}

void xhci__runt_write64(struct xhci *ctrl, os_uint32 offset, os_uint32 low, os_uint32 high)
{
    os_uint32 *mem = (void *)ctrl->b;
    mem[(ctrl->rts_offset + offset) >> 2] = low;
    if (ctrl->hccparams1 & 1)
    {
        mem[(ctrl->rts_offset + offset + 4) >> 2] = high;
    }
}

/* 0_xHCI_Rev1_2b.pdf: 7.2 xHCI Supported Protocol Capability */
os_uint32 xhci__get_proto_offset(struct xhci *ctrl, os_uint32 list_off, os_uint32 version, os_uint32 *offset, os_uint32 *count, os_uint16 *flags)
{
    os_uint32 next;
    os_uint32 item_next;

    *count = 0;
    do
    {
        item_next = xhci__read_ecp_next_capability_pointer(ctrl, list_off);
        next = item_next ? (list_off + (item_next << 2)) : 0;

        if (xhci__read_ecp_capability_id(ctrl, list_off) == XHCI_ECP_ID_SUPPORTED_PROTOCOL &&
            xhci__read_ecp_revision_major(ctrl, list_off) == version)
        {
            *offset = xhci__read_ecp_compatible_port_offset(ctrl, list_off) - 1;
            *count = xhci__read_ecp_compatible_port_count(ctrl, list_off);
            *flags = xhci__read_ecp_protocol_defined(ctrl, list_off);
            return next;
        }
        list_off = next;
    } while (list_off != 0);

    return 0;
}

void xhci__stop_legacy(struct xhci *ctrl, os_uint32 list_off)
{
    os_uint32 next;
    os_uint32 item_next;

    do
    {
        item_next = xhci__read_ecp_next_capability_pointer(ctrl, list_off);
        next = item_next ? (list_off + (item_next << 2)) : 0;
        if (xhci__read_ecp_capability_id(ctrl, list_off) == XHCI_ECP_ID_LEGACY_SUPPORT)
        {
            xhci__write_usblegsup(ctrl, list_off,
                                  xhci__read_usblegsup(ctrl, list_off) | XHCI_ECP_HC_OS_OWNED_SEMAPHORE);
            rtos__thread_sleep(2);
            if ((xhci__read_usblegsup(ctrl, list_off) &
                 (XHCI_ECP_HC_BIOS_OWNED_SEMAPHORE | XHCI_ECP_HC_OS_OWNED_SEMAPHORE)) ==
                XHCI_ECP_HC_OS_OWNED_SEMAPHORE)
            {
                k__printf("Legacy xHCI owned by OS\n");
                return;
            }
            else
            {
                k__printf("FAILURE: Legacy xHCI owned by BIOS\n");
            }
            break;
        }
        list_off = next;
    } while (list_off != 0);

    return;
}

os_uint32 xhci__alloc(struct xhci *ctrl, os_uint32 size, os_uint32 aligment, os_uint32 boundary)
{
    os_uint32 r;
    os_uint32 b;

    if (ctrl->heap == NULL)
    {
        ctrl->heap_size = XHCI_HEAP_SIZE;
        ctrl->heap = rtos__heap_alloc(NULL, ctrl->heap_size);
        ctrl->heap_current = (os_uint32)ctrl->heap;
    }

    ctrl->heap_current = (ctrl->heap_current + aligment - 1) & ~(aligment - 1);

    if (boundary > 0)
    {
        b = (ctrl->heap_current + boundary - 1) & ~(boundary - 1);
        if (ctrl->heap_current + size > b)
        {
            ctrl->heap_current = b;
        }
    }

    r = ctrl->heap_current;
    ctrl->heap_current += size;
    if (ctrl->heap_current >= ((os_uint32)ctrl->heap) + ctrl->heap_size)
    {
        k__printf("PANIC: cannot allocate xHCI memory\n");
        while (1)
        {
        }
        return 0;
    }
    k__memset((void *)r, 0, size);
    return r;
}

os_uint32 xhci__create_ring(struct xhci *ctrl, os_uint32 trbs)
{
    os_uint32 a;
    os_uint32 p;

    a = xhci__alloc(ctrl, trbs * sizeof(struct xhci_trb), 64, 0x10000);
    p = a + ((trbs - 1) * sizeof(struct xhci_trb));

    xhci__write_trb_data_buffer_pointer(ctrl, p, a, 0);
    xhci__write_trb_08(ctrl, p, 0);
    xhci__write_trb_0C(ctrl, p, TRB_TYPE_LINK | TRB_CYCLE_BIT);
    return a;
}

os_uint32 xhci__create_event_ring(struct xhci *ctrl, os_uint32 trbs, os_uint32 *ret)
{
    os_uint32 tbl;
    os_uint32 a;

    tbl = xhci__alloc(ctrl, 64, 64, 0);
    a = xhci__alloc(ctrl, trbs * sizeof(struct xhci_trb), 64, 0x10000);
    xhci__write_trb_data_buffer_pointer(ctrl, tbl, a, 0);
    xhci__write_trb_08(ctrl, tbl, trbs);
    xhci__write_trb_0C(ctrl, tbl, 0);
    *ret = a;
    return tbl;
}

os_uint32 xhci__disable_port(struct xhci *ctrl, os_uint32 port)
{
    xhci__write_oper_portsc(ctrl, port, PORTSC_PED);
    rtos__thread_sleep(2); /* FIXME */

    if ((xhci__read_oper_portsc(ctrl, port) & PORTSC_PP) != 0)
    {
        k__printf("Cannot disable port %d (%d, power %d)\n", port,
                  (xhci__read_oper_portsc(ctrl, port) & PORTSC_PLS_MASK) >> 5,
                  xhci__read_oper_portsc(ctrl, port) & PORTSC_PP);
        return 0;
    }
    return 1;
}

os_uint32 xhci__reset_port(struct xhci *ctrl, os_uint32 port)
{
    os_uint32 ret = 0;
    os_uint32 val;
    os_uint32 timeout;

    if ((xhci__read_oper_portsc(ctrl, port) & PORTSC_PP) == 0)
    {
        xhci__write_oper_portsc(ctrl, port, PORTSC_PP);
        rtos__thread_sleep(2);
        if ((xhci__read_oper_portsc(ctrl, port) & PORTSC_PP) == 0)
        {
            k__printf("Cannot power up port %d\n", port);
            return 0;
        }
    }

    val = xhci__read_oper_portsc(ctrl, port);
    xhci__write_oper_portsc(ctrl, port, PORTSC_PP | PORTSC_CSC | PORTSC_PEC | PORTSC_OCC | PORTSC_PRC | PORTSC_PLC);
    if (!ctrl->ports[port].is_usb3)
    {
        xhci__write_oper_portsc(ctrl, port, PORTSC_PP | PORTSC_PR);
    }
    else
    {
        xhci__write_oper_portsc(ctrl, port, PORTSC_PP | PORTSC_WPR);
    }
    val = xhci__read_oper_portsc(ctrl, port);

    timeout = 500;
    while (timeout)
    {
        /* xhci__event(ctrl);*/
        val = xhci__read_oper_portsc(ctrl, port);
        if (val & (PORTSC_PRC /*| PORTSC_WRC*/))
        {
            break;
        }
        timeout--;
        /* rtos__thread_sleep(1);*/
        k__usleep(1000);
    }
    if (timeout > 0)
    {
        /* xhci__event(ctrl);
          rtos__thread_sleep(1);
          */
        k__usleep(3000);
        val = xhci__read_oper_portsc(ctrl, port);
        if (val & PORTSC_PED)
        {
            xhci__write_oper_portsc(ctrl, port, PORTSC_PP | PORTSC_CSC | PORTSC_PEC | PORTSC_OCC | PORTSC_PRC | PORTSC_PLC);
            k__printf("Reset USB port %d ok : %x\n", port, val);
            ret = 1;
        }
    }
    else
    {
        k__printf("Reset USB port %d timeout : %x\n", port, val);
    }
    return ret;
}

void xhci__write_doorbell(struct xhci *ctrl, os_uint32 slot_id, os_uint32 val)
{
    k__sfence();
    xhci__phy_write32(ctrl, ctrl->db_offset + (slot_id * (sizeof(os_uint32))), val);
}

void xhci__reset(struct xhci *ctrl)
{
    os_uint32 timeout;

    /* set Host Controller Reset  and wait */
    xhci__write_oper_usbcommand_host_controller_reset(ctrl, 1);
    timeout = 50;
    while (xhci__read_oper_usbcommand_host_controller_reset(ctrl))
    {
        timeout--;
        if (timeout == 0)
        {
            k__printf("Cannot reset xHCI\n");
            return;
        }
        rtos__thread_sleep(1); /* 10ms */
    }
}

void xhci__event(struct xhci *ctrl)
{
    os_uint32 val;
    os_uint32 interrupter = 0;
    os_uint32 last_addr;
    os_uint32 address;
    struct xhci_trb event;
    struct xhci_trb origin;

    val = xhci__read_oper_usbstatus(ctrl);
    if (val & 0x1000) /* internal error */
    {
        k__printf(" # %x", val);
        xhci__reset(ctrl);
        xhci__write_oper_usbcommand(ctrl, USBCOMMAND_RUN | USBCOMMAND_INTE | USBCOMMAND_HSEE);
    }
    else if (val)
    {
        xhci__write_oper_usbstatus(ctrl, val);
        k__printf(" @ %x", val);
        /*__builtin_trap();*/
    }

    last_addr = ctrl->cur_event_ring_addr;
    val = xhci__read_runt_interrupter_iman(ctrl, interrupter);
    if ((val & (IMAN_IP | IMAN_IE)) == (IMAN_IP | IMAN_IE))
    {
        /*__builtin_trap();*/
        xhci__write_runt_interrupter_iman(ctrl, interrupter, val);

        event.param[0] = xhci__read_trb_data_buffer_pointer(ctrl, last_addr, &event.param[1]);
        event.status = xhci__read_trb_08(ctrl, last_addr);
        event.command = xhci__read_trb_0C(ctrl, last_addr);
        k__printf(" Interrupt USB(%x) %x %x %x!!       ", val, event.command, event.status, event.param[0]);
        while ((event.command & 1) == ctrl->cur_event_ring_cycle)
        {
            k__printf(" SPLI (%x) %x %x\n", event.command, event.status, event.param[0]);

            if ((event.command & (1 << 2)) == 0)
            {
                k__printf(" COMMMMMM (%d) %x\n", (event.command >> 10) & 0x3F, (event.status >> 24) & 0x7F);
                switch ((event.status >> 24) & 0x7F)
                {
                case 1: /* TRB success*/
                    address = event.param[0];
                    switch ((event.command >> 10) & 0x3F)
                    {
                    case 33: /* COMMAND COMPLETITION */
                        origin.param[0] = xhci__read_trb_data_buffer_pointer(ctrl, address, &origin.param[1]);
                        origin.status = xhci__read_trb_08(ctrl, address);
                        origin.command = xhci__read_trb_0C(ctrl, address);
                        switch ((origin.command >> 10) & 0x3F)
                        {
                        case 9: /* ENABLE SLOT*/
                            origin.command &= 0x00FFFFFF;
                            origin.command |= (event.command & 0xFF000000); /* return slot ID (1 based)*/
                            origin.status = event.status;
                            break;
                        default:
                            origin.status = event.status;
                        }
                        origin.status |= (1 << 31); /* DONE */
                        xhci__write_trb_data_buffer_pointer(ctrl, address, origin.param[0], origin.param[1]);
                        xhci__write_trb_08(ctrl, address, origin.status);
                        xhci__write_trb_0C(ctrl, address, origin.command);
                        break;
                    case 34: /* PORT STATUS CHANGE */
                        k__printf("port %d status change: %x ", (event.param[0] >> 24) & 0xFF, (event.status >> 24) & 0xff);
                        break;
                    }
                    break;
                }
            }
            else
            {
                k__printf(" YOUK (%d) %x\n", (event.command >> 10) & 0x3F, (event.status >> 24) & 0x7F);

                switch ((event.command >> 10) & 0x3F)
                {
                case 32: /* TRANS EVENT*/
                    xhci__phy_write32(ctrl, event.param[0], event.status | (1 << 31));
                    break;
                default:
                    xhci__phy_write32(ctrl, event.param[0], event.status | (1 << 31));
                    break;
                }
            }
            last_addr = ctrl->cur_event_ring_addr;
            ctrl->cur_event_ring_addr += sizeof(struct xhci_trb);
            event.param[0] = xhci__read_trb_data_buffer_pointer(ctrl, ctrl->cur_event_ring_addr, &event.param[1]);
            event.status = xhci__read_trb_08(ctrl, ctrl->cur_event_ring_addr);
            event.command = xhci__read_trb_0C(ctrl, ctrl->cur_event_ring_addr);
        }
        xhci__write_runt_interrupter_dequeue(ctrl, interrupter, last_addr | (1 << 3), 0);
    }
}



os_uint32 xhci__send_command(struct xhci *ctrl, struct xhci_trb *trb, os_uint32 ring_it)
{
    os_uint32 org_trb_addr = ctrl->cmnd_trb_addr;
    os_uint32 cmnd;
    os_uint32 timer;

    k__printf("T %x %x %x %x\n", trb->command, sizeof(struct xhci_trb), trb->param[0], ctrl->cmnd_ring_addr);

    xhci__write_trb_data_buffer_pointer(ctrl, ctrl->cmnd_trb_addr, trb->param[0], trb->param[1]);
    xhci__write_trb_08(ctrl, ctrl->cmnd_trb_addr, trb->status);
    xhci__write_trb_0C(ctrl, ctrl->cmnd_trb_addr, trb->command | ctrl->cmnd_trb_cycle);

    ctrl->cmnd_trb_addr += sizeof(struct xhci_trb);

    cmnd = xhci__read_trb_0C(ctrl, ctrl->cmnd_trb_addr);

    if (xhci__is_trb_link_command(cmnd))
    {
        xhci__write_trb_0C(ctrl, ctrl->cmnd_trb_addr, (cmnd & ~1) | ctrl->cmnd_trb_cycle);
        ctrl->cmnd_trb_addr = ctrl->cmnd_ring_addr;
        ctrl->cmnd_trb_cycle ^= 1;
    }

    if (ring_it)
    {
        xhci__write_doorbell(ctrl, 0, 0);

        /* wait for interrupt */
        timer = 200;
        xhci__event(ctrl);
        while (timer && (xhci__read_trb_08(ctrl, org_trb_addr) & (1 << 31)) == 0)
        {
            xhci__event(ctrl);
            rtos__thread_sleep(1);
            xhci__event(ctrl);
            timer--;
        }
        if (timer == 0)
        {
            k__printf(" USB xHCI Command Interrupt wait timed out.");
            return 1;
        }
        else
        {
            trb->param[0] = xhci__read_trb_data_buffer_pointer(ctrl, org_trb_addr, &trb->param[1]);
            trb->status = xhci__read_trb_08(ctrl, org_trb_addr);
            trb->command = xhci__read_trb_0C(ctrl, org_trb_addr);
            trb->status &= ~(1 << 31);
        }
    }

    return 0;
}

os_uint32 xhci__get_descriptor(struct xhci *ctrl, int port)
{
    os_uint32 sc;
    os_uint32 speed;
    /*os_uint32 max_packet;*/

    sc = xhci__read_oper_portsc(ctrl, port);
    speed = (sc >> 10) & 0x0F;
    switch (speed)
    {
    case 1:
        k__printf("USB port %d: full speed\n", port);
        break;
    case 2:
        k__printf("USB port %d: low speed\n", port);
        break;
    case 3:
        k__printf("USB port %d: high speed\n", port);
        break;
    case 4:
        k__printf("USB port %d: super speed\n", port);
        break;
    default:
        k__printf("USB port %d: cannot get speed\n", port);
        return 1;
    }
    ctrl->trb.param[0] = 0;
    ctrl->trb.param[1] = 0;
    ctrl->trb.status = 0;
    xhci__trb_enable_slot_command(ctrl);
    xhci__send_command(ctrl, &ctrl->trb, 1);
    return 0;
}

/*

*/
void xhci__init_ctrl(struct xhci *ctrl)
{
    os_uint32 v;
    os_uint32 timeout;
    os_uint32 ndp;
    os_uint32 ext_caps_off;
    os_uint32 next;
    os_uint16 flags;
    os_uint32 max_slots = 1;
    os_uint32 event_ring_addr;
    os_uint32 cnt;
    os_uint32 offset;
    os_intn ports_usb2;
    os_intn ports_usb3;
    os_uint32 scratch_buff_array_start;
    os_uint32 scratch_buff_start;
    os_uint32 max_scratch_buffs;
   /* os_uint16 vendor;
    os_uint16 device;*/
    os_uint32 interrupt = 0;
    int i, j;

    ctrl->heap = NULL;
    ctrl->b = XHCI_DEFAULT_BASE0;


    v = xhci__read_caps_iversion(ctrl);
    k__printf("xHCI Version %x (at %x)\n", v, ctrl->b);

    ctrl->op_base_off = xhci__read_caps_cap_length(ctrl);

    ctrl->hccparams1 = xhci__read_caps_hccparams1(ctrl);
    ctrl->hccparams2 = xhci__read_caps_hccparams2(ctrl);
    ctrl->hcsparams1 = xhci__read_caps_hcsparams1(ctrl);
    ctrl->hcsparams2 = xhci__read_caps_hcsparams2(ctrl);
    ctrl->rts_offset = xhci__read_caps_rtsoff(ctrl);
    ctrl->db_offset = xhci__read_caps_dboff(ctrl);
    ctrl->context_size = xhci__read_caps_hccparams1_context_size(ctrl);
    ext_caps_off = xhci__read_caps_hccparams1_ext_caps_off(ctrl);

    /* wait "controller not ready" comes to 0*/
    timeout = 100;
    while (xhci__read_oper_usbstatus_controller_not_ready(ctrl))
    {
        timeout--;
        if (timeout == 0)
        {
            k__printf("xHCI doesn't wakeup %x ctx size %d\n",
                      xhci__read_oper_usbstatus(ctrl), ctrl->context_size);
            return;
        }
        rtos__thread_sleep(1); /* 10ms */
    }

    xhci__reset(ctrl);

    xhci__stop_legacy(ctrl, ext_caps_off);

    ndp = get_field_hcsparams1_max_ports(ctrl->hcsparams1);
    if (ndp > XHCI_MAX_PORT)
    {
        ndp = XHCI_MAX_PORT;
    }

    ports_usb2 = 0;
    /* parse extended capability list */
    next = ext_caps_off;
    offset = 0;
    while (next)
    {
        next = xhci__get_proto_offset(ctrl, next, 2, &offset, &cnt, &flags);
        if (cnt > 0)
        {
            for (i = 0; i < cnt; i++)
            {
                ctrl->ports[offset + i].offset = ports_usb2;
                ports_usb2++;
                ctrl->ports[offset + i].is_usb3 = 0;
                if (flags & 0x2)
                {
                    ctrl->ports[offset + i].is_high_speed_only = 1;
                }
            }
        }
    }

    ports_usb3 = 0;
    next = ext_caps_off;
    offset = 0;
    while (next)
    {
        next = xhci__get_proto_offset(ctrl, next, 3, &offset, &cnt, &flags);
        if (cnt > 0)
        {
            for (i = 0; i < cnt; i++)
            {
                ctrl->ports[offset + i].offset = ports_usb3;
                ports_usb3++;
                ctrl->ports[offset + i].is_usb3 = 1;
            }
        }
    }
    k__printf("Found %d USB2 root hub ports / %d\n", ports_usb2, ndp);

    for (i = 0; i < ndp; i++)
    {
        for (j = 0; j < ndp; j++)
        {
            if (ctrl->ports[i].offset == ctrl->ports[j].offset &&
                ctrl->ports[i].is_usb3 != ctrl->ports[j].is_usb3)
            {
                ctrl->ports[i].other_port_num = j;
                ctrl->ports[j].other_port_num = i;
                ctrl->ports[i].has_pair = 1;
                ctrl->ports[j].has_pair = 1;
            }
        }
    }
    for (i = 0; i < ndp; i++)
    {
        if (!ctrl->ports[i].has_pair ||
            (ctrl->ports[i].has_pair && ctrl->ports[i].is_usb3))
        {
            ctrl->ports[i].is_active = 1;
        }
    }
    ctrl->page_size = xhci__read_oper_usbpagesize(ctrl);

    ctrl->max_slots = xhci__read_caps_hcsparams1_max_slots(ctrl);

    ctrl->dcbaap_start = xhci__alloc(ctrl, 2048, 64, ctrl->page_size);
    xhci__write_oper_usbdcbaap(ctrl, ctrl->dcbaap_start, 0);
    k__printf("dcbaap_start %x max: %x\n", ctrl->dcbaap_start, ctrl->max_slots);

    max_scratch_buffs = xhci__read_caps_hcsparams2_max_scratch_buffs(ctrl);
    if (max_scratch_buffs > 0)
    {
        scratch_buff_array_start = xhci__alloc(ctrl, max_scratch_buffs * 8, 64, ctrl->page_size);
        scratch_buff_start = xhci__alloc(ctrl, max_scratch_buffs * ctrl->page_size, ctrl->page_size, 0);

        xhci__phy_write64(ctrl, ctrl->dcbaap_start, scratch_buff_array_start, 0);
        k__printf("scratch_buff_array_start %x\n", scratch_buff_array_start);
        for (i = 0; i < max_scratch_buffs; i++)
        {
            xhci__phy_write64(ctrl, scratch_buff_array_start + i * 8, scratch_buff_start + i * ctrl->page_size, 0);
        }
    }

    ctrl->cmnd_trb_addr = xhci__create_ring(ctrl, 128);
    ctrl->cmnd_ring_addr = ctrl->cmnd_trb_addr;
    ctrl->cmnd_trb_cycle = TRB_CYCLE_BIT;
    k__printf("CMD RING %x\n", ctrl->cmnd_ring_addr);

    xhci__write_oper_usbdcrcr(ctrl, ctrl->cmnd_ring_addr | TRB_CYCLE_BIT, 0);
    xhci__write_oper_usbconfig(ctrl, max_slots);
    xhci__write_oper_usbdnctrl(ctrl, DNCTRL_N1); /* notification enable */

    event_ring_addr = xhci__create_event_ring(ctrl, 4096, &ctrl->cur_event_ring_addr);
    ctrl->cur_event_ring_cycle = 1;

    k__printf("EV RING %x\n", event_ring_addr);

    xhci__write_runt_interrupter_iman(ctrl, interrupt, IMAN_IP | IMAN_IE);
    xhci__write_runt_interrupter_imod(ctrl, interrupt, 0x0);
    xhci__write_runt_interrupter_tab_size(ctrl, interrupt, 1);
    xhci__write_runt_interrupter_dequeue(ctrl, interrupt, ctrl->cur_event_ring_addr | DEQUEUE_EHB, 0);
    xhci__write_runt_interrupter_address(ctrl, interrupt, event_ring_addr, 0);

    xhci__write_oper_usbstatus(ctrl, USBSTATUS_HSE | USBSTATUS_EINT | USBSTATUS_PCD | USBSTATUS_SRE);
    xhci__write_oper_usbcommand(ctrl, USBCOMMAND_RUN | USBCOMMAND_INTE | USBCOMMAND_HSEE);

    rtos__thread_sleep(10);
    for (i = 0; i < ndp; i++)
    {
        if (ctrl->ports[i].is_usb3)
        {
            if (xhci__reset_port(ctrl, i))
            {
                ctrl->ports[i].is_active = 1;
                k__printf("Reset USB3 port %d OK\n", i);
            }
            else
            {
                ctrl->ports[i].is_active = 0;
                ctrl->ports[ctrl->ports[i].other_port_num].is_active = 1;
            }
        }
    }

    for (i = 0; i < ndp; i++)
    {
        if (!ctrl->ports[i].is_usb3 &&
            (!ctrl->ports[i].has_pair || ctrl->ports[i].is_active))
        {
            if (xhci__reset_port(ctrl, i))
            {
                ctrl->ports[i].is_active = 1;
                if (ctrl->ports[i].has_pair)
                {
                    ctrl->ports[ctrl->ports[i].other_port_num].is_active = 0;
                }
                k__printf("Reset USB2 port %d OK\n", i);
            }
            else
            {
                ctrl->ports[i].is_active = 0;
            }
        }
    }

    for (i = 0; i < ndp; i++)
    {
        if (ctrl->ports[i].is_active)
        {
            xhci__get_descriptor(ctrl, i);
        }
    }
    k__printf("xHCI status %x\n",
              xhci__read_oper_usbstatus(ctrl));
}

#endif

os_intn xhci__find_device(os_intn idx, os_uint32 *bus, os_uint32 *slot, os_uint32 *function)
{
    os_uint32 vendor;
    os_uint32 class_, subc, pif;
    os_intn i = 0;
    /*os_uint32 device;*/

    for (*bus = 0; *bus < PCI_MAX_BUS; (*bus)++)
    {
        for (*slot = 0; *slot < PCI_MAX_SLOT; (*slot)++)
        {
            for (*function = 0; *function < PCI_MAX_FUNCTION; (*function)++)
            {
                vendor = pci__cfg_read_vendor(*bus, *slot, *function);
                if (vendor != PCI_VENDOR_NO_DEVICE)
                {
                    /*device = pci__cfg_read_device(*bus, *slot, *function);*/
                    class_ = pci__cfg_read_class(*bus, *slot, *function);
                    subc = pci__cfg_read_subclass(*bus, *slot, *function);
                    pif = pci__cfg_read_prog_if(*bus, *slot, *function);

                    if (class_ == PCI_CLASS_SERIAL_BUS_CONTROLLER && subc == PCI_SUBCLASS_USB && pif == PCI_PROG_IF_XHCI_CONTROLLER)
                    {
                        if (i == idx)
                        {
                            return 0;
                        }
                        i++;
                    }
                }
            }
        }
    }
    return 1;
}

void xhci__irq(os_uint32 n)
{
    /*struct xhci_ *ctrl = xhci__ctrl + 0;
     xhci__event(ctrl);*/
}

static struct xhci_ctrl xhci__ctrl[XHCI_MAX_CONTROLLER];
static struct xhci_root xhci__root[XHCI_MAX_CONTROLLER];

void xhci__init()
{
    os_intn r;
    os_intn i;
    struct xhci_ctrl *ctrl = &xhci__ctrl[0];
    os_uint32 bus, slot, function;
    os_uint32 base0, base1, irq, size;
    os_uint32 vendor, device;
    /*  pci__cfg_write16(ctrl->bus, ctrl->slot, ctrl->function, 4, 0x0006);*/

    /*k__memset(ctrl->ports, 0, sizeof(ctrl->ports));
     */
    /*   pci__cfg_write8(ctrl->bus, ctrl->slot, ctrl->function, 0x61, 0x20);*/

    for (i = 0; i < XHCI_MAX_CONTROLLER; i++)
    {
        ctrl = &xhci__ctrl[i];
        ctrl->root = (var)&xhci__root[i];
        r = xhci__find_device(i, &bus, &slot, &function);
        if (r == 0)
        {
            base0 = pci__cfg_read_base_addr(bus, slot, function, 0) & ~0xF;
            base1 = pci__cfg_read_base_addr(bus, slot, function, 1);
#ifndef __RPI400__
            if (base0 && !base1)
            {
                base0 = base0 & ~0xF;
            } else {
                base0 = XHCI_DEFAULT_BASE0;
                base1 = 0;
            }
#else
            base0 = XHCI_DEFAULT_BASE0;
#endif

            irq = pci__cfg_read_interrupt_line(bus, slot, function);
            size = pci__mem_range(bus, slot, function, 0x10);

            vendor = pci__cfg_read_vendor(bus, slot, function);
            device = pci__cfg_read_device(bus, slot, function);
            k__printf("xHCI vendorID:%x deviceID:%x\n", vendor, device);
            if ((vendor == 0x8086) &&
                (pci__cfg_read_revision(bus, slot, function) == 4) &&
                ((device == 0x1E31) ||
                 (device == 0x0F35) ||
                 (device == 0x9C31) ||
                 (device == 0x8C31) ||
                 (device == 0x8CB1) ||
                 (device == 0x9CB1)))
            {
                k__printf("Reroute ports from EHCI to XHCI\n");
                /* https://forum.osdev.org/viewtopic.php?p=260528&sid=65b0915514edb12fb954ee35ee188770#p260528 */
                pci__cfg_write32(bus, slot, function, 0xD8, 0xFFFFFFFF);
                pci__cfg_write32(bus, slot, function, 0xD0, 0xFFFFFFFF);
            }
            ctrl->no_deconfigure = 0;
            if (((vendor == 0x1B73) && (device == 0x1009)) ||
                ((vendor == 0x104C) && (device == 0x8241)))
            {
                ctrl->no_deconfigure = 1;
            }

            k__printf("XHCI found: bus %d slot %d func %d base 0x%x:%x irq %d size 0x%x\n",
                      bus, slot, function,
                      base1, base0, irq, size);
            xhci_ctrl__init((void*)ctrl, base0, base1, irq, size);
            break;
        }
    }
    k__printf("done\n");
}

#include "xhci_ctrl.c"
