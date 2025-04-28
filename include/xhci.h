#ifndef XHCI_H
#define XHCI_H

/* https://github.com/fysnet/FYSOS/blob/master/main/usb/utils/include/xhci.h */
#include "klib.h"
#include "pci.h"
#include "rtos.h"

#define CAPLENGTH 0
#define HCIVERSION 2
#define HCSPARAMS1 4
#define HCSPARAMS2 8
#define HCSPARAMS3 12
#define HCCPARAMS1 16
#define DBOFF 20
#define RTSOFF 24
#define HCCPARAMS2 28
#define USBCMD 0
#define USBSTS 4
#define PAGESIZE 8
#define CRCR 24
#define DCBAAP 48
#define CONFIG 56
#define CYCLE_BIT 1
#define TRB_TYPE_LINK (6 << 10) 
#define TRB_TYPE_NORMAL (1 << 10) 
#define TRB_TYPE_SETUP_STAGE (2 << 10) 
#define TRB_TYPE_DATA_STAGE (3 << 10) 
#define TRB_TYPE_STATUS_STAGE (4 << 10) 
#define TRB_TYPE_EVENT_DATA (7 << 10) 
#define ERSTSZ 0x28
#define ERSTBA 0x30
#define ERDP 0x38
#define PORTSC 0x400


struct xhci_port {
	var offset;
	var major;
	var hso;
	var is_active;
	var other;
};

struct xhci_trb {
	k__u32 a_lo;
	k__u32 a_hi;
	k__u32 status;
	k__u32 control;
};

struct xhci_st {
	k__u32 rsba_lo;
	k__u32 rsba_hi;
	k__u32 rss;
	k__u32 rsvdz;
};

struct xhci_device {
	var input_ctx; /* Input slots */
	var device_ctx; /* Output slots*/
	var endpoints[33];
	var enqueue[33];
};

struct xhci {
	var base;
	var cap;
	var op;
	var rt;
	var db;
	var xecp;
	var ac64;
	var csz;
	var max_slots;
	var max_ports;
	var ist;
	var no_deconfigure;
	var hciversion;
	var timeout;

	var dcbaap;
	var page_size;
	var max_scratchpad;
	var scratchpad;
	var slots;
	struct xhci_device *devices;
	var tmp_descriptor;
	struct xhci_trb *tmp_status;

	var command_ring;
	var command_pcs;
	var command_ep;
	var event_ring_st;
	var event_begin;
	var event_end;
	var event_ccs;
	var event_dp;

	var ports;
};

var xhci__init_controller(struct xhci *self, var base0, 
		var base1, var irq, var size);
var xhci__event(struct xhci *self, var id);

#endif
