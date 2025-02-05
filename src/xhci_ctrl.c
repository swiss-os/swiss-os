
#include "../include/xhci.h"


var aligned_alloc(var size, var align, var boundary)
{
	var  bunch, al, start, end;
	if (size > boundary) {
		boundary = 0;
	}
	al = boundary + align;
	if ((boundary - size) > size) {
		if ((boundary - align) > align) {
			al = size;
		}
	}
	bunch = (var)rtos__heap_alloc(NULL, (size + al));
	if (boundary > 0) {
		start = bunch & (~(boundary-1));
		end = (bunch + size) & ~(boundary-1);
		if (end != start) {
			bunch = end;
		}
	}

	if (align > 0) {
		bunch = bunch & ~(align-1);
	}
	return bunch;
}

var rd8(var base, var offset)
{
	base = base + offset;
	return ((*((volatile var*)((volatile char*)(base)))) >> (base&3)) & 0xFF;
}
var rd16(var base, var offset)
{
	var  align;
	base = base + offset;
	align = base & 3;
	base = base - align;
	if (align == 0) {
		return ((*((volatile var*)((volatile char*)(base))))) & 0xFFFF;
	} else if (align == 1) {
		return ((*((volatile var*)((volatile char*)(base)))) >> 8) & 0xFFFF;
	} else if (align == 2) {
		return ((*((volatile var*)((volatile char*)(base)))) >> 16) & 0xFFFF;
	} else {
		return (((*((volatile var*)((volatile char*)(base)))) >> 24) & 0xFF) +
			(((*((volatile var*)((volatile char*)(base+4)))) << 8) & 0xFF00);
	}
	
}
var rd32(var base, var offset)
{
	var  align;
	base = base + offset;
	align = base & 3;
	base = base - align;
	if (align == 0) {
		return ((*((volatile var*)((volatile char*)(base))))) & 0xFFFFFFFF;
	} else if (align == 1) {
		return (((*((volatile var*)((volatile char*)(base)))) >> 8) & 0x00FFFFFF) +
			(((*((volatile var*)((volatile char*)(base+4)))) << 24) & 0xFF000000);
	} else if (align == 2) {
		return (((*((volatile var*)((volatile char*)(base)))) >> 16) & 0x0000FFFF) +
			(((*((volatile var*)((volatile char*)(base+4)))) << 16) & 0xFFFF0000);
	} else {
		return (((*((volatile var*)((volatile char*)(base)))) >> 24) & 0x000000FF) +
			(((*((volatile var*)((volatile char*)(base+4)))) << 8) & 0xFFFFFF00);
	}
}

var wr32(var base, var offset, var value)
{
	*((volatile var*)((volatile unsigned char*)(base)+(offset))) = value;
	return 0;
}

var xhci_ctrl__init(struct xhci_ctrl *self, var base0, var base1, var irq, var size)
{
	var  v;
	struct xhci_root *root_hub;
	self->base = base0;
	self->cap = self->base;
	self->op = self->base + rd8(self->cap, CAPLENGTH);
	self->rt = self->base + rd32(self->cap, RTSOFF) ;
	self->db = self->base + rd32(self->cap, DBOFF) ;
	self->hciversion = rd16(self->cap, HCIVERSION);
	self->xecp = self->base + rd16(self->cap, (HCCPARAMS1 + 2)) * 4;
	self->ac64 = rd8(self->cap, HCCPARAMS1) & 1;
	self->csz = (rd8(self->cap, HCCPARAMS1) & 0x07) > 2;
	self->max_slots = rd8(self->cap, HCSPARAMS1);
	self->ist = rd8(self->cap, HCSPARAMS2) & 0x0F ;
	root_hub = (void*)self->root;

//	((var(*)())xhci_root__init)((var)root_hub, self);
	//((var(*)())xhci_ctrl__parse_extended)(self);
	//((var(*)())xhci_ctrl__reset)(self);
	v = ((16777216 * 25) / 16777216) % 256;
	k__printf("%x",(v) & 0xFF);
	k__printf("%s","\nhello world\n");
	k__printf("%x",(self->hciversion));
	k__printf("%s","\n");
	return 0;
}
