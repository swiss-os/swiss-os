#ifndef VAR_TYPEDEF_
#define VAR_TYPEDEF_
typedef long var;
#endif

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
#define CRCR 24
#define DCBAAP 48
#define CONFIG 56



struct xhci_port {
	var root_hub;
	var major;
	var speed;
};

struct xhci_sw_ring {
	var size;
	var begin;
	var end;
	var enqueue;
	var dequeue;
	var running;
};

struct xhci_root {
	var ctrl;
	var max_ports;
	var ports;
	var event_ring;
};

struct xhci_ctrl {
	var base;
	var cap;
	var op;
	var rt;
	var db;
	var xecp;
	var ac64;
	var csz;
	var max_slots;
	var ist;
	var root;
	var no_deconfigure;
	var hciversion;
	var timeout;
};

var xhci_ctrl__init(struct xhci_ctrl *self, var base0, var base1, var irq, var size);


#endif
