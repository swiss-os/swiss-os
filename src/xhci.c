/*
 
             MMXXV April 24 PUBLIC DOMAIN by JML

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

static struct xhci xhci__ctrl[XHCI_MAX_CONTROLLER];

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

                    if (class_ == PCI_CLASS_SERIAL_BUS_CONTROLLER && 
				    subc == PCI_SUBCLASS_USB && 
				    pif == PCI_PROG_IF_XHCI_CONTROLLER)
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
	struct xhci *ctrl = &xhci__ctrl[0];
	xhci__event(ctrl, (var)n);
}


void xhci__init()
{
    os_intn r;
    os_intn i;
    struct xhci *ctrl = &xhci__ctrl[0];
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
            xhci__init_controller((void*)ctrl, base0, base1, irq, size);
            break;
        }
    }
    k__printf("USB init done\n");
}

var xhci__reset(struct xhci *self)
{
	var timeout, tmp;
	
	/* Host controller reset (HCRST) */
	tmp = k__rd32(self->op, USBCMD);
	k__wr32(self->op, USBCMD, tmp | (1 << 1));
	timeout = 50;
	while (k__rd32(self->op, USBCMD) & (1 << 1)) {
		timeout--;
		if (timeout <= 0) {
			k__printf("Cannot reset xHCI. USBCMD:0x%x\n",
					k__rd32(self->op, USBCMD));
			return -1;
		}
		k__usleep(10000);
	}
	return 0;
}

var xhci__run(struct xhci *self)
{
	var timeout, tmp;
	
	/* Run/Stop (R/S) */
	tmp = k__rd32(self->op, USBCMD);
	k__wr32(self->op, USBCMD, tmp | 1); 
	k__usleep(10000);
	timeout = 50;
	while ((k__rd32(self->op, USBCMD) & 1) == 0) {
		timeout--;
		if (timeout <= 0) {
			k__printf("Cannot start xHCI. USBCMD:0x%x\n",
					k__rd32(self->op, USBCMD));
			return -1;
		}
		k__usleep(10000);
	}
	return 0;
}

var xhci__wait(struct xhci *self, struct xhci_trb *p, var timeout)
{
	var c;
	char *code = "Success";
	while (timeout > 0) {
		timeout--;
		xhci__event(self, 0);
		if ((volatile var)(p->status) & (1 << 31)) {
			break;
		}
		k__usleep(10000);
	}
	if (timeout < 1) {
		return -1;
	}
	p->status &= 0x7FFFFFFF;
	c = ((p->status >> 24) & 0x7F);
	switch (c) {
	case 1:
		return 0;
	case 0: code = "Invalid"; break;
	case 2: code = "Data Buffer Error"; break;
	case 3: code = "Babble Detected Error"; break;
	case 4: code = "USB transaction Error"; break;
	case 5: code = "TRB Error"; break;
	case 6: code = "Stall Error"; break;
	case 7: code = "Ressource Error"; break;
	case 8: code = "Bandwidth Error"; break;
	case 9: code = "No slots availablei Error"; break;
	case 10: code = "Invalid Stream Type Error"; break;
	case 11: code = "Slot not Enabled Error"; break;
	case 12: code = "Endpoint not enabled Error"; break;
	case 13: 
		 code = "Short Packet"; 
		 return 0;
		 break;
	case 14: code = "Ring Underrun"; break;
	case 15: code = "Ring Overrun"; break;
	case 16: code = "VF Event Ring Full Error"; break;
	case 17: code = "Parameter Error"; break;
	case 18: code = "Bandwidth Overrun Error"; break;
	case 19: code = "Context state Error"; break;
	case 20: code = "No ping response Error"; break;
	case 21: code = "Event Ring Full Error"; break;
	case 22: code = "Incmpatible Device Error"; break;
	case 23: code = "Missed service Error"; break;
	case 24: code = "Command Ring Stopped"; break;
	case 25: code = "Command Aborted"; break;
	case 26: code = "Stopped"; break;
	case 27: code = "Stopped - Length Invalid"; break;
	case 28: code = "Stopped - Short Packet"; break;
	case 29: code = "Max Exit Latency Too Large Error"; break;
	case 30: code = "Reserved"; break;
	case 31: code = "Isoch Buffer Overrun"; break;
	case 32: code = "Event Lost Error"; break;
	case 33: code = "Undefined Error"; break;
	case 34: code = "Invaild Stream ID Error"; break;
	case 35: code = "Secondary Bandwidth Error"; break;
	case 36: code = "Split Transaction Error"; break;
	default:
		if (c <= 191) {
			code = "Reserved";
		} else if (c <= 223) {
			code = "Vendor D. Error";
		} else {
			code = "Vendor D. Info";
		}	
	}
	k__printf("Failure (%d) %s : ", c, code);
	return -1;
}

var xhci__ep_enqueue(struct xhci *self, var slot_id, var ep_num, var *cb)
{
	struct xhci_trb *trb;
	var c;
	c = self->devices[slot_id].enqueue[ep_num];
	trb = (void*) (c & ~0x01);
	c &= 1;
	while ((trb->control & 1) != c) {
		switch (trb->control & (0x3F << 10)) {
		case TRB_TYPE_LINK:
			trb->control = (trb->control & 0xFFFFFFFE) 
				| c;
			if (trb->control & (1 << 1)) { /* TC */
				if (c) {
					c = 0;
				} else {
					c = 1;
				}
			}

			trb = (void*)trb->a_lo;
			self->devices[slot_id].enqueue[ep_num] = 
				((var)(trb)) | c;
			break;
		default:
			self->devices[slot_id].enqueue[ep_num] = 
				((var)(trb + 1)) | c;
			*cb = c; /* set cycle bit */
			return (var)(trb);
		}
	}
	*cb = c;
	/* ring is full */
	k__printf("Endpoint ring is full\n");
	return 0;
}



var xhci__cmd_enqueue(struct xhci *self)
{
	struct xhci_trb *trb;
	trb = (void*)self->command_ep;
	while ((trb->control & 1) != self->command_pcs) {
		switch (trb->control & (0x3F << 10)) {
		case TRB_TYPE_LINK:
			trb->control = (trb->control & 0xFFFFFFFE) 
				| self->command_pcs;
			if (trb->control & (1 << 1)) { /* TC */
				if (self->command_pcs) {
					self->command_pcs = 0;
				} else {
					self->command_pcs = 1;
				}
			}

			trb = (void*)trb->a_lo;
			self->command_ep = (var)(trb);
			break;
		default:
			self->command_ep = (var)(trb + 1);
			return (var)(trb);
		}
	}
	/* ring is full */
	k__printf("Command ring is full\n");
	return 0;
}


var xhci__doorbell(struct xhci *self, var slot, var val)
{
	k__sfence();
	k__wr32(self->db, slot * 4, val);
	return 0;
}

var xhci__cmd_address_device(struct xhci *self, var slot_id, var bsr)
{
	var input_ctx;
	var slot_ctx;
	var ep0_ctx;
	var device_ctx;
	var d_slot_ctx;
	var d_ep0_ctx;
	struct xhci_trb *p;
	p = (void*)xhci__cmd_enqueue(self);
	if (!p) {
		return -1;
	}
	device_ctx = self->devices[slot_id].device_ctx;
	if (!device_ctx) {
		return -1;
	}
	d_slot_ctx = device_ctx;
	d_ep0_ctx = device_ctx + self->csz;

	/* Input context*/
	input_ctx = self->devices[slot_id].input_ctx;
	if (!input_ctx) {
		input_ctx = k__aligned_alloc(33 * self->csz, 64, 
				self->page_size); 
		self->devices[slot_id].input_ctx = input_ctx;
	}
	slot_ctx = input_ctx + self->csz;
	ep0_ctx = input_ctx + self->csz * 2;
	k__memset((void*)input_ctx, 0, 33 * self->csz);

	((var*)input_ctx)[1] = 3; /* Add Context flags A0 A1 */

	((var*)slot_ctx)[0] = ((var*)d_slot_ctx)[0];
	((var*)slot_ctx)[1] = ((var*)d_slot_ctx)[1];
	((var*)slot_ctx)[2] = ((var*)d_slot_ctx)[2];
	((var*)slot_ctx)[3] = ((var*)d_slot_ctx)[3];

	((var*)ep0_ctx)[0] = ((var*)d_ep0_ctx)[0];
	((var*)ep0_ctx)[1] = ((var*)d_ep0_ctx)[1];
	((var*)ep0_ctx)[2] = ((var*)d_ep0_ctx)[2];
	((var*)ep0_ctx)[3] = ((var*)d_ep0_ctx)[3];

	p->a_lo = input_ctx;
	p->a_hi = 0;
	p->status = 0;
	p->control = (11 << 10) /* Address device command */ 
		| (slot_id << 24)
		| (bsr << 9) /* block set address request flag */
		| self->command_pcs;
	xhci__doorbell(self, 0, 0);
	if (xhci__wait(self, p, 50)) {
		k__printf("Command address device %d\n", slot_id);
		return -1;
	}
	((var*)d_slot_ctx)[0] = ((var*)slot_ctx)[0]; 
	((var*)d_slot_ctx)[1] = ((var*)slot_ctx)[1]; 
	((var*)d_slot_ctx)[2] = ((var*)slot_ctx)[2]; 
	((var*)d_slot_ctx)[3] = ((var*)slot_ctx)[3]; 
	((var*)d_ep0_ctx)[0] = ((var*)ep0_ctx)[0]; 
	((var*)d_ep0_ctx)[1] = ((var*)ep0_ctx)[1]; 
	((var*)d_ep0_ctx)[2] = ((var*)ep0_ctx)[2]; 
	((var*)d_ep0_ctx)[3] = ((var*)ep0_ctx)[3]; 
	return 0;
}


var xhci__cmd_enable_slot(struct xhci *self, var type, var *slot_id)
{
	struct xhci_trb *p;
	*slot_id = -1;
	p = (void*)xhci__cmd_enqueue(self);
	if (!p) {
		return -1;
	}
	p->a_lo = 0;
	p->a_hi = 0;
	p->status = 0;
	p->control = (9 << 10) /* enable slot */
		| ((type & 0x1F) << 16) /* slot type */
		| self->command_pcs;
	xhci__doorbell(self, 0, 0);
	if (xhci__wait(self, p, 50)) {
		k__printf("Command enable slot\n");
		return -1;
	}
	*slot_id = (p->control >> 24) & 0xFF;
	return 0;
}


var xhci__cmd_no_op(struct xhci *self)
{
	struct xhci_trb *p;
	p = (void*)xhci__cmd_enqueue(self);
	if (!p) {
		return -1;
	}
	p->a_lo = 0;
	p->a_hi = 0;
	p->status = 0;
	p->control = (23 << 10) /* no op */
		| self->command_pcs;
	xhci__doorbell(self, 0, 0);
	if (xhci__wait(self, p, 50)) {
		k__printf("Command no op\n");
		return -1;
	}
	return 0;
}

var xhci__create_ring(struct xhci *self, var nb_trb)
{
	var a;
	struct xhci_trb *p;
	a = k__aligned_alloc(nb_trb * sizeof(struct xhci_trb), 64, self->page_size);
	k__memset((void*)a, 0, nb_trb * sizeof(struct xhci_trb));
	p = (void*) (a + ((nb_trb - 1) * sizeof(struct xhci_trb)));
	p->a_lo = (k__u32)a;
	p->a_hi = 0;
	p->status = 0;
	p->control = TRB_TYPE_LINK  
		| (1 << 1); /* TC toggle cyle */
	return a;
}

var xhci__create_event_ring(struct xhci *self, var nb_trb, var inter)
{
	var a;
	struct xhci_st *s;
	s = (void*) k__aligned_alloc(sizeof(struct xhci_st), 64, 0);
	a = k__aligned_alloc(nb_trb * sizeof(struct xhci_trb), 64, self->page_size);
	k__memset((void*)a, 0, nb_trb * sizeof(struct xhci_trb));
	s->rsba_lo = a;
	s->rsba_hi = 0;
	s->rss = nb_trb;
	s->rsvdz = 0;

	k__wr32(self->rt, ERSTSZ + inter * 32, 1);
	k__wr32(self->rt, ERSTBA + inter * 32, (var)s);
	k__wr32(self->rt, ERSTBA + 4 + inter * 32, 0);

	return (var)s;
}

var xhci__command_event(struct xhci *self, struct xhci_trb *trb,
	       	struct xhci_trb *p)
{
	if (!p) {
		return -1;
	}
	switch ((p->control >> 10) & 0x3F) {
	case 9: /* enable slot */
		p->control &= 0x00FFFFFF;
		/* slot id */
		p->control |= trb->control & 0xFF000000;
		break;
	case 11: /* Address device command */ 
	case 23: /* no op */
		break;
	default:
		k__printf("USB ok %x %d->%d\n", trb->status, 
				(p->control >> 10) & 0x3F,
				(trb->control >> 10) & 0x3F);
		break;
	}
	return 0;
}

var xhci__event_set_status(struct xhci *self, struct xhci_trb *p,
	struct xhci_trb *trb)
{
	if (trb->control & (1 << 2)) { /* Event data ED*/
		if (trb->a_lo) {
			*((var*)trb->a_lo) = trb->status |
				(1 << 31); /*done */
		}
	} else {
		p = (void*)trb->a_lo;
		if (p) {
			p->status = trb->status;
			p->status |= (1 << 31); /* done */ 
		}
	}
	return 0;
}
	
var xhci__event_dequeue(struct xhci *self, var inter)
{
	var last;
	struct xhci_trb *p;
	struct xhci_trb *trb;
	last = self->event_dp;
	trb = (void*)last;
	while ((trb->control & 1) == self->event_ccs) {
		switch (trb->control & (0x3F << 10)) {
		case (33 << 10): /*TRB_TYPE_COMMAND_COMPLETION_EVENT*/
			if (trb->control & (1 << 2)) { /* Event data ED*/
				p = NULL;
			} else {
				p = (void*)trb->a_lo;
			}
			switch ((trb->status >> 24) & 0x7F) {
			case 1:
			case 13: 
				xhci__command_event(self, trb, p);
				break;
			default:
				/* fail */
				break;
			}
			xhci__event_set_status(self, p, trb);
			last = (var)trb;
			trb++;
			break;
		case (34 << 10): /* port status change */
			k__printf("port %d status %x\n", 
				(trb->a_lo >> 24) & 0xFF,
				(trb->status >> 24) & 0xFF);
			last = (var)trb;
			trb++;
			break;
		case (32 << 10): /* Transfer Event */
			xhci__event_set_status(self, p, trb);
			last = (var)trb;
			trb++;
			break;
		case 0: /* Reserved */
			xhci__event_set_status(self, p, trb);
			last = (var)trb;
			trb++;
			break;
		default:
			k__printf("%d USB_event: %d %x\n", self->event_ccs, 
					(trb->control >> 10) & 0x3F, (var)trb);
			last = (var)trb;
			trb++;
		}
		if ((var)trb == self->event_end) {
			self->event_ccs ^= 1;
			trb = (void*)self->event_begin;
		}
	
	}
	self->event_dp = (var)trb;
	k__wr32(self->rt, ERDP + inter * 32, last);
	k__wr32(self->rt, ERDP + inter * 32 + 4, 0);
	return 0;
}


var xhci__stop_legacy(struct xhci *self)
{
	var next;
	var ptr;
	var xecp;
	var usblegsup;

	if (self->xecp == self->base) {
		return -1;
	}
	xecp = self->xecp;
    	while (xecp) {
		ptr = k__rd8(xecp, 1);
		if (ptr) {
			next = xecp + ptr * 4;
		} else {
			next = 0;
		}
		/* capability id */
		switch (k__rd8(xecp, 0)) {
		case 1: /* USB LEGACY Support */
			usblegsup = k__rd32(xecp, 0);
			/* bios owned */
			if (usblegsup & (1 << 16)) {
				/* request ownership */	
				k__wr32(xecp, 0, usblegsup | (1 << 24));
				k__usleep(2 * 10000);
				usblegsup = k__rd32(xecp, 0);
				if ((usblegsup & 0x01010000) == (1 << 24)) {
                			k__printf("Legacy xHCI owned by OS\n");
				} else {
                			k__printf("FAILURE: Legacy xHCI ",
						"owned by BIOS 0x%x\n",
						usblegsup);
				}
			}
			break;
		default:
			break;
		}
		xecp = next;
	}
	return 0;
}

var xhci__event(struct xhci *self, var id)
{
	var usbsts;
	usbsts = k__rd32(self->op, USBSTS);
	if (usbsts & 1) {
		/* HCHalted */
		xhci__reset(self);
		xhci__run(self);
		return 0;
	} else if (usbsts) {
		xhci__event_dequeue(self, 0);
		/* clear EINT */
		k__wr32(self->op, USBSTS, usbsts | (1 << 3));
	} else {
        	k__printf("*");
	}
	return 0;
}

var xhci__proto_cap(struct xhci *self, var version, 
	var *offset, var *count, var *flags)
{
	var next;
	var ptr;
	var xecp;
	var xhcispc;
	var name;

	*count = 0;
	if (self->xecp == self->base) {
		return -1;
	}
	xecp = self->xecp;
    	while (xecp) {
		ptr = k__rd8(xecp, 1);
		if (ptr) {
			next = xecp + ptr * 4;
		} else {
			next = 0;
		}
		/* capability id */
		switch (k__rd8(xecp, 0)) {
		case 2: /* SUPPORTED PROTOCOL */
			xhcispc = k__rd32(xecp, 0);
			/* Major revision  2 or 3*/
			if (((xhcispc >> 24) & 0xFF) == version) {
				/* name string "USB " */
				name = k__rd32(xecp, 4);
				if (name == 0x20425355) {
					*offset = k__rd8(xecp, 8);
					*count = k__rd8(xecp, 9);
					*flags = k__rd8(xecp, 10);
					return 0;
				}
			}
			break;
		default:
			break;
		}
		xecp = next;
	}
	return -1;
}

var xhci__ep_status_stage(struct xhci *self, var slot_id, var ep_num,
		var dir, var status_addr)
{
	var c = 0;
	struct xhci_trb *p;
	p = (void*)xhci__ep_enqueue(self, slot_id, ep_num, &c);
	if (!p) {
		return -1;
	}
	p->a_lo = 0;
	p->a_hi = 0;
	p->status = 0;
	p->control =  c
		| (dir << 16) /* TRT Transfer Type : 0 no data, 2 OUT, 3 IN*/
		| TRB_TYPE_STATUS_STAGE 
		| (1 << 4); /* Chain bit CH*/

	p = (void*)xhci__ep_enqueue(self, slot_id, ep_num, &c);
	if (!p) {
		return -1;
	}
	*((var*)status_addr) = 0;
	p->a_lo = status_addr;
	p->a_hi = 0;
	p->status = 0;
	p->control = c /* cycle bit */
		| TRB_TYPE_EVENT_DATA
//		| (1 << 9) /* Block Event Interrupt BEI */
		| (1 << 5); /* Interrupt on completion IOC */

	return 0;
}


var xhci__ep_setup_stage(struct xhci *self, var slot_id, var ep_num,
		var dir, var value, var request, var request_type,
		var length, var index)
{
	var c = 0;
	struct xhci_trb *p;
	p = (void*)xhci__ep_enqueue(self, slot_id, ep_num, &c);
	if (!p) {
		return -1;
	}
	p->a_lo = request_type | (request << 8) | (value << 16);
	p->a_hi = index | (length << 16);
	p->status = 8; /* transfer length */
	p->control =  c
		| (dir << 16) /* TRT Transfer Type : 0 no data, 2 OUT, 3 IN*/
		| TRB_TYPE_SETUP_STAGE 
		| (1 << 6); /* IDT immediate data */
	return 0;
}

var xhci__ep_data_stage(struct xhci *self, var slot_id, var ep_num,
		var dir, var data, var trb_type, var size_, var max_packet, 
		var status_addr)
{
	struct xhci_trb *p;
	var i = 0;
	var c = 0;
	var size = size_;
	var remain = (k__div((size + (max_packet - 1)), max_packet) - 1);
	if (remain < 0) {
		remain = 0;
	}
	while (size > 0) {
		p = (void*)xhci__ep_enqueue(self, slot_id, ep_num, &c);
		if (!p) {
			return -1;
		}
		p->a_lo = data;
		p->a_hi = 0;
		if (size < max_packet) {
			p->status = size; /* TRB transfer length */
		} else {
			p->status = max_packet; /*TRB transfer length */
		}
		p->status |= (remain << 17); /* TD size*/
		p->control = c /* cycle bit */
			| (dir << 16) /* TRT Transfer Type : 0 OUT, 1 IN*/
			| trb_type 
			| (1 << 4) /* Chain bit CH*/
			| ((remain == 0) << 1); /* Evaluate Next TRB ENT*/

		data += max_packet;
		size -= max_packet;
		i++;
		remain--;
		trb_type = TRB_TYPE_NORMAL;
		dir = 0;
	}


	p = (void*)xhci__ep_enqueue(self, slot_id, ep_num, &c);
	if (!p) {
		return -1;
	}
	*((var*)status_addr) = 0;
	p->a_lo = status_addr;
	p->a_hi = 0;
	p->status = 0;
	p->control = c /* cycle bit */
		| TRB_TYPE_EVENT_DATA
//		| (1 << 9) /* Block Event Interrupt BEI */
		| (1 << 5); /* Interrupt on completion IOC */
	return 0;
}


var xhci__ep_init(struct xhci *self, var ep, var slot_id, var ep_num,
		var max_packet_size, var ep_type, var direction, var speed,
		var ep_interval)
{
	var *ctx = (void*)ep;
	if (ep_type != 4) {
		return -1;
	}
	ctx[0] = (ep_interval << 16);
	ctx[1] = (max_packet_size << 16) 
		| (ep_type << 3)
		| (3 << 1); /*CErr Error count */

	/* TR Dequeue Pointer Lo*/
	ctx[2] = self->devices[slot_id].endpoints[ep_num];
	if (!ctx[2]) {
		ctx[2] = xhci__create_ring(self, 64);
		self->devices[slot_id].endpoints[ep_num] = ctx[2];
	}
	ctx[2] |= 1; /*DCS*/
	self->devices[slot_id].enqueue[ep_num] = ctx[2];
	ctx[3] = 0;
	ctx[4] = 8; /* Average TRB Length */
	return 0;
}

var xhci__slot_init(struct xhci *self, var slot, var port, var speed)
{
	var *ctx = (void*)slot;
	ctx[0] = (speed << 20) 
		| (1 << 27) /* context entries 1 (the ep) */
		;
	ctx[1] = port << 16; /* root hub port number */ 
	return 0;
}

var xhci__get_descriptor(struct xhci *self, var descriptor, var len, 
		var slot_id, var max_packet)
{
	if (xhci__ep_setup_stage(self, slot_id, 
		1/*ep_num: Control EP 0*/, 
		3/* IN direction*/, 
		(1 << 8)/* DEVICE descriptor type. value 9.3*/, 
	       	6/*GET_DESCRIPTOR request 9.4*/, 
		USB_GET_STANDARD_DEVICE_REQUEST /*request_type*/,
		len/*length*/, 
		0/*index*/))
	{
		k__printf("setup stage fail %d\n", slot_id);
		return -1;
	}

	if (xhci__ep_data_stage(self, slot_id, 
		1/*ep_num: Control EP 0*/, 
		1/* IN direction*/, 
		self->tmp_descriptor, /* data */
		TRB_TYPE_DATA_STAGE, /* trb_type */
		len,
		max_packet,
		(var)&self->tmp_status->status))
	{
		k__printf("data stage fail %d\n", slot_id);
		return -1;
	}



	xhci__doorbell(self, slot_id, 1); /*ep_num = 1: Control EP 0*/ 
	if (xhci__wait(self, self->tmp_status, 10)) {
		k__printf("#", slot_id);
	}

	if (xhci__ep_status_stage(self, slot_id, 
		1/*ep_num: Control EP 0*/, 
		1/* IN direction*/, 
		(var)&self->tmp_status->status))
	{
		k__printf("status stage fail %d\n", slot_id);
		return -1;
	}

	xhci__doorbell(self, slot_id, 1); /*ep_num = 1: Control EP 0*/ 
	if (xhci__wait(self, self->tmp_status, 10)) {
		k__printf("fail GET_DESCRIPTOR %d\n", slot_id);
		return -1;
	}

	usb__print_device_descriptor(self, slot_id, self->tmp_descriptor);
	return 0;
}

var xhci__check_port(struct xhci *self, var n)
{
	var portsc;
	portsc = k__rd32(self->op, PORTSC + 0x10 * (n-1));
	if (portsc & (1 << 1)) { /* PED */
		portsc = (1 << 9) /*PP*/
				| (1 << 17) /*CSC*/
				| (1 << 18) /*PEC*/
				| (1 << 20) /*OCC*/
				| (1 << 21) /*PRC*/
				| (1 << 22) /*PLC*/	
				;
		k__wr32(self->op, PORTSC + 0x10 * (n-1), portsc);
		k__printf("Port %d reset ok\n", n);
		return 0;
	}
	k__printf("Port %d error 0x%x\n", n, portsc);
	return -1;
}

var xhci__reset_port(struct xhci *self, var n)
{
	struct xhci_port *ports;
	var portsc;
	ports = (void*)self->ports;
	portsc = k__rd32(self->op, PORTSC + 0x10 * (n-1));
	/* port power */
	if ((portsc & (1 << 9)) == 0) {
		portsc |= (1 << 9);
		k__wr32(self->op, PORTSC + 0x10 * (n-1), portsc);
		k__usleep(20000);
		portsc = k__rd32(self->op, PORTSC + 0x10 * (n-1));
		if ((portsc & (1 << 9)) == 0) {
			k__printf("Cannot power up port %d (%x)\n", n, portsc);
			return -1;
		}
	}
	portsc = (1 << 9) /*PP*/
		| (1 << 17) /*CSC*/
		| (1 << 18) /*PEC*/
		| (1 << 20) /*OCC*/
		| (1 << 21) /*PRC*/
		| (1 << 22) /*PLC*/	
		;
	k__wr32(self->op, PORTSC + 0x10 * (n-1), portsc);
	if (ports[n].major != 3) {
		portsc = (1 << 9) /*PP*/
			| (1 << 4); /*PR*/
	} else {
		portsc = (1 << 9) /*PP*/
			| (1 << 31); /*WPR*/
	}
	k__wr32(self->op, PORTSC + 0x10 * (n-1), portsc);
	return 0;
}


var xhci__init_port(struct xhci *self, var port_id)
{
	var slot_id;
	var device_ctx;
	var ep0_ctx;
	var portsc;
	var speedval = 0;
	var max_packet = 0;
	char *speed = "unknown";
	var timeout;
	var descriptor;
	struct xhci_trb *status;

	if (xhci__cmd_enable_slot(self, 0, &slot_id)) {
		return -1;
	}
	if (slot_id <= 0 || slot_id >= self->max_slots) {
		return -1;
	}

	portsc = k__rd32(self->op, PORTSC + 0x10 * (port_id-1));
	speedval = (portsc >> 10) & 0x0F;
	switch (speedval) {
	case 1:
		speed = "full";
		max_packet = 64;
		break;
	case 2:
		speed = "low";
		max_packet = 8;
		break;
	case 3:
		speed = "high";
		max_packet = 64;
		break;
	case 4:
		speed = "super";
		max_packet = 512;
		break;
	}

	device_ctx = self->devices[slot_id].device_ctx;
	if (device_ctx == 0) {
		/* allocate 2 Output contexts: slot and control endpoint, 
		 * a context is 64 or 32 bytes */
		device_ctx = k__aligned_alloc(self->csz * 2, 
				32, self->page_size);
		((var*)self->slots)[slot_id << 1] = device_ctx;
		self->devices[slot_id].device_ctx = device_ctx;
	}
	k__memset((void*)device_ctx, 0, 2 * self->csz);
	k__wr32(self->dcbaap, slot_id * 8, device_ctx);
	k__wr32(self->dcbaap, slot_id * 8 + 4, 0);

	xhci__slot_init(self, device_ctx, port_id, speedval);
	ep0_ctx = device_ctx + self->csz;
	xhci__ep_init(self, ep0_ctx, slot_id, 
			1/*index: Control EP 0*/, max_packet, 
			4/*type: control ep*/, 0, speedval, 0);

	if (xhci__cmd_address_device(self, slot_id, 1)) {
		return -1;
	}
	descriptor = self->tmp_descriptor;
	status = (void*)self->tmp_status;
	k__memset((void*)descriptor, 0, USB_DEVICE_DESCRIPTOR_SIZE);
	if (xhci__get_descriptor(self, descriptor, 8, slot_id, max_packet)) {
		return -1;
	}
	max_packet = 1 << (*((k__u8*)(descriptor+7)));

	((var*)ep0_ctx)[1] &= 0x0000FFFF;
	((var*)ep0_ctx)[1] |= (max_packet << 16); 

	xhci__reset_port(self, port_id);
	timeout = 50;
	while (timeout > 0) {
		if (!xhci__check_port(self, port_id)) {
			break;
		}
		timeout--;
		k__usleep(10000);
	}
	if (timeout <= 0) {
		return -1;
	}
	k__usleep(50000);

	if (xhci__cmd_address_device(self, slot_id, 1)) {
		return -1;
	}
	if (xhci__get_descriptor(self, descriptor, USB_DEVICE_DESCRIPTOR_SIZE,
			slot_id, max_packet)) 
	{
		return -1;
	}

	k__printf("USB port %d: %s speed, slot %d.\n", port_id, speed, slot_id);
	return 0;
}
var xhci__devices_init(struct xhci *self)
{
	var i, j, offset, count, flags;
	var usb3, usb2;
	var usbsts, timeout;
	struct xhci_port *ports;
	ports = (void*)self->ports;
	usb3 = 0;
	usb2 = 0;
	xhci__proto_cap(self, 3, &offset, &count, &flags); 
	for (i = offset; i <= self->max_ports && i < (offset + count); i++) {
		ports[i].major = 3;
		ports[i].hso = 0;
		ports[i].other = 0;
		ports[i].offset = usb3;
		usb3++;
	}	
	xhci__proto_cap(self, 2, &offset, &count, &flags); 
	for (i = offset; i <= self->max_ports && i < (offset + count); i++) {
		ports[i].major = 2;
		ports[i].hso = 0;
		if (flags & (1 << 1)) {
			/* high speed only */
			ports[i].hso = 1;
		}
		ports[i].other = 0;
		ports[i].offset = usb2;
		usb2++;

	}	
	for (i = 1; i <= self->max_ports; i++) {
		for (j = i + 1; j <= self->max_ports; j++) {
			if (ports[i].offset == ports[j].offset &&
				ports[i].major != ports[j].major)
			{
				ports[i].other = j;
				ports[j].other = i;
			}
		}
		ports[i].is_active = 0;
	}
	for (i = 1; i <= self->max_ports; i++) {
		if (ports[i].major == 2) {
			xhci__reset_port(self, i);
		}
	}
	timeout = 50;
	while (timeout > 0) {
		timeout--;
		k__usleep(10000);
	}

	for (i = 1; i <= self->max_ports; i++) {
		if (ports[i].major == 2) {
			if (!xhci__check_port(self, i)) {
				ports[i].is_active = 1;
			} else {
				ports[i].is_active = 0;
			}
		}
	}

	for (i = 1; i <= self->max_ports; i++) {
		if (ports[i].major == 3) {
			if (ports[i].other > 0 && 
					ports[ports[i].other].is_active != 0)
			{
				continue;
			}
			xhci__reset_port(self, i);
		}
	}
	timeout = 50;
	while (timeout > 0) {
		timeout--;
		k__usleep(10000);
	}

	for (i = 1; i <= self->max_ports; i++) {
		if (ports[i].major == 3) {
			if (ports[i].other > 0 && 
					ports[ports[i].other].is_active != 0)
			{
				continue;
			}
			if (!xhci__check_port(self, i)) {
				ports[i].is_active = 1;
			} else {
				ports[i].is_active = 0;
			}
		}
		if (ports[i].is_active) {
			xhci__init_port(self, i);
		}
	}

	usbsts = k__rd32(self->op, USBSTS);
	k__printf(" %d ports (%d USB2, %d USB3) USBSTS:0x%x\n",
			self->max_ports, usb2, usb3, usbsts);
	return 0;
}

var xhci__init_controller(struct xhci *self, var base0, 
		var base1, var irq, var size)
{
	var  timeout, i;
	struct xhci_st *s;

	self->base = base0;
	self->cap = self->base;
	self->op = self->base + k__rd8(self->cap, CAPLENGTH);
	self->rt = self->base + k__rd32(self->cap, RTSOFF) ;
	self->db = self->base + k__rd32(self->cap, DBOFF) ;
	self->hciversion = k__rd16(self->cap, HCIVERSION);
	self->xecp = self->base + k__rd16(self->cap, (HCCPARAMS1 + 2)) * 4;
	self->ac64 = k__rd8(self->cap, HCCPARAMS1) & 1;
	self->csz = 32;
	if ((k__rd8(self->cap, HCCPARAMS1) & 0x04)) {
		self->csz = 64;
	}
	self->max_slots = k__rd8(self->cap, HCSPARAMS1);
	self->max_ports = k__rd8(self->cap, HCSPARAMS1 + 3);
	self->ist = k__rd8(self->cap, HCSPARAMS2) & 0x0F ;
	self->max_scratchpad = ((k__rd32(self->cap, HCSPARAMS2) >> 27) & 0x1F)
		| (((k__rd32(self->cap, HCSPARAMS2) >> 21) & 0x1F) << 5);
	/* wait until the Controller Not Ready (CNR) flag is 0 */ 
	timeout = 100;
	while ((k__rd32(self->op, USBSTS) & (1 << 11)) != 0) {
		timeout--;
		if (timeout <= 0) {
			k__printf("xHCI doesn't wakeup. USBSTS:0x%x\n",
					k__rd32(self->op, USBSTS));
			return -1;
		}
		k__usleep(10000);
	}
	
	xhci__reset(self);

	xhci__stop_legacy(self);

	self->ports = k__aligned_alloc((self->max_ports + 1) * 
			sizeof(struct xhci_port), 64, 0);

	/* Program the Max Device Slots Enabled (MaxSlotsEn) */
	k__wr32(self->op, CONFIG, self->max_slots);

	/* Program the Device Context Base Address Array Pointer (DCBAAP) */
	self->page_size = k__rd16(self->op, PAGESIZE) * 4096; 
	self->dcbaap = k__aligned_alloc(2048, 64, self->page_size);
	k__memset((void*)self->dcbaap, 0, 2048);
	k__wr32(self->op, DCBAAP, self->dcbaap & ~0x3F);
	k__wr32(self->op, DCBAAP + 4, 0);

	/* temporary buffer for endpoint descriptor requests */
	self->tmp_descriptor = k__aligned_alloc(512, 64, self->page_size);
	/* temporary status trbfor endpoint descriptor requests */
	self->tmp_status = (void*)k__aligned_alloc(
			sizeof(struct xhci_trb), 64, self->page_size);

	/* set Command Ring Control Register (CRCR)  pointing to the 
	  * starting address of the first TRB of the Command Ring.*/
	self->command_ring = xhci__create_ring(self, 64);
	k__wr32(self->op, CRCR, self->command_ring | CYCLE_BIT);
	k__wr32(self->op, CRCR + 4, 0);
	self->command_ep = self->command_ring;
	self->command_pcs = 1;
	/* Defining the Event Ring */
	/* xHC is the producer, OS is the consumer */
	/* why 128 ?  must be between  16 to 4096*/  
	self->event_ring_st = xhci__create_event_ring(self, 128, 0); 
	s = (void*)self->event_ring_st;
	self->event_begin = s[0].rsba_lo;
	self->event_end = self->event_begin + 
		s[0].rss * sizeof(struct xhci_trb);
	self->event_ccs = 1;
	self->event_dp = ((struct xhci_st *)self->event_ring_st)->rsba_lo;
	xhci__event_dequeue(self, 0);

	/* scratchpad buffer */
	if (self->max_scratchpad > 0) {
		self->scratchpad = k__aligned_alloc(self->max_scratchpad * 8,
			64, self->page_size);
		((var*)self->dcbaap)[0] = self->scratchpad;
		((var*)self->dcbaap)[1] = 0;

		for (i = 0; i < self->max_scratchpad * 2; i += 2) {
			((var*)self->scratchpad)[i] = k__aligned_alloc(
				self->page_size, self->page_size, 
				self->page_size);
			((var*)self->scratchpad)[i+1] = 0;
		}
	}
	
	self->slots = k__aligned_alloc((self->max_slots + 1) * 8, 
			64, self->page_size);
	k__memset((void*)self->slots, 0, (self->max_slots + 1) * 8);
	self->devices = (void*)k__alloc(self->max_slots * 
			sizeof(struct xhci_device));
	k__memset((void*)self->devices, 0, 
			(self->max_slots + 1) * sizeof(struct xhci_device));

	xhci__run(self);

	xhci__devices_init(self);

	k__printf("xhci version %x / page size %dkB\n",
			self->hciversion, self->page_size / 1024);
	timeout = 2;
	while (timeout > 0) {
		timeout--;
		xhci__cmd_no_op(self);
	}
	return 0;
}

