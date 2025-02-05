/* trip to C generated */
#ifndef TRIP_IMPLEMENTATION
typedef long var;
#endif
#ifndef TRIP_IMPLEMENTATION
struct xhci_port;
var xhci_port__init(var,var);
var xhci_port__set_major(var,var);
var xhci_port__set_speed(var,var);
struct xhci_sw_ring;
var xhci_sw_ring__init(var,var);
var xhci_sw_ring__restart(var);
struct xhci_trb;
var xhci_trb__reset(var);
struct xhci_root;
var xhci_root__init(var,var);
var xhci_root__set_ports_protocol(var,var,var,var);
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
var mod(var,var);
var aligned_alloc(var,var,var);
var clamp8(var);
var clamp16(var);
var clamp24(var);
var rd8(var,var);
var rd16(var,var);
var rd32(var,var);
var wr32(var,var,var);
struct xhci_ctrl;
var xhci_ctrl__init(var,var,var,var,var);
var xhci_ctrl__parse_extended(var);
var xhci_ctrl__sleep(var);
var xhci_ctrl__reset(var);
var xhci_ctrl__max_ports(var);
#endif
#ifdef TRIP_IMPLEMENTATION
struct xhci_port {
	var root_hub;
	var major;
	var speed;
};
var xhci_port__init(var self, var rh)
{
	struct xhci_port *__self = (void*)self;(void)__self;
	__self->root_hub = rh;
	__self->major = 0;
	return 0;
}
var xhci_port__set_major(var self, var v)
{
	struct xhci_port *__self = (void*)self;(void)__self;
	__self->major = v;
	return 0;
}
var xhci_port__set_speed(var self, var v)
{
	struct xhci_port *__self = (void*)self;(void)__self;
	__self->speed = v ;
	return 0;
}
struct xhci_sw_ring {
	var size;
	var begin;
	var end;
	var enqueue;
	var dequeue;
	var running;
};
var xhci_sw_ring__init(var self, var size_)
{
	struct xhci_sw_ring *__self = (void*)self;(void)__self;
	__self->size = size_;
	__self->begin = ((var(*)())aligned_alloc)((__self->size * 4 * 4), 64, 65536) ;
	__self->end = __self->begin + (4 * __self->size);
	((var(*)())xhci_sw_ring__restart)(self);
	return 0;
}
var xhci_sw_ring__restart(var self)
{
	struct xhci_sw_ring *__self = (void*)self;(void)__self;
	var  i;
	struct xhci_trb *t;
	__self->enqueue = __self->begin;
	__self->dequeue = __self->begin;
	i = 0;
	while (1) {
		var __cond = __self->size - i ;
		if (__cond > 0) {
			(*((var*)&t)) = ((var*)(__self->begin))[i];
			((var(*)())xhci_trb__reset)((var)t);
			i = i + 1 ;
		} else { break; }
	}
	__self->running = ((var)1);
	return 0;
}
struct xhci_trb {
	var parameter0;
	var parameter1;
	var status;
	var control;
};
var xhci_trb__reset(var self)
{
	struct xhci_trb *__self = (void*)self;(void)__self;
	__self->parameter0 = 0;
	__self->parameter1 = 0;
	__self->status = 0;
	__self->control = 0;
	return 0;
}
struct xhci_root {
	var ctrl;
	var max_ports;
	var ports;
	var event_ring;
};
var xhci_root__init(var self, var ctrl_)
{
	struct xhci_root *__self = (void*)self;(void)__self;
	var  i;
	struct xhci_ctrl *c;
	struct xhci_port *p;
	struct xhci_sw_ring *ev;
	(*((var*)&c)) = ctrl_;
	__self->ctrl = (*((var*)&c));
	__self->max_ports = ((var(*)())xhci_ctrl__max_ports)((var)c);
	__self->ports = (var)malloc(sizeof(var) * (__self->max_ports));
	i = 0;
	while (1) {
		var __cond = __self->max_ports - i ;
		if (__cond > 0) {
			(*((var*)&p)) = (var)malloc(sizeof(struct xhci_port));
			((var*)(__self->ports))[i] = (*((var*)&p));
			((var(*)())xhci_port__init)((var)p, self);
			i = i + 1;
		} else { break; }
	}
	__self->event_ring = (var)malloc(sizeof(struct xhci_sw_ring));
	(*((var*)&ev)) = __self->event_ring;
	((var(*)())xhci_sw_ring__init)((var)ev, __self->max_ports);
	printf("%s","\nend\n");
	return 0;
}
var xhci_root__set_ports_protocol(var self, var cp_off, var cp_count, var major)
{
	struct xhci_root *__self = (void*)self;(void)__self;
	var  i;
	struct xhci_port *p;
	i = cp_off;
	while (1) {
		var __cond = i - (cp_off + cp_count) ;
		if (__cond > 0) {
			(*((var*)&p)) = ((var*)(__self->ports))[i];
			((var(*)())xhci_port__set_major)((var)p, major);
			i = i + 1;
		} else { break; }
	}
	return 0;
}
var mod(var numerator, var denominator)
{
	var  rest, den;
	{
		var __cond = denominator ;
		if (__cond <= 0) {
			return 0;
		}
	}
	{
		var __cond = numerator ;
		if (__cond <= 0) {
			return 0;
		}
	}
	rest = numerator;
	den = denominator;
	while (1) {
		var __cond = den * 2 ;
		if (__cond > 0) {
			den = den * 2;
		} else { break; }
	}
	while (1) {
		var __cond = rest - denominator ;
		if (__cond >= 0) {
			{
				var __cond = rest - den ;
				if (__cond >= 0) {
					rest = rest - den;
				}
			}
			den = den / 2;
		} else { break; }
	}
	return rest;
}
var aligned_alloc(var size, var align, var boundary)
{
	var  bunch, al, start, end;
	{
		var __cond = size - boundary ;
		if (__cond > 0) {
			boundary = 0;
		}
	}
	al = boundary + align;
	{
		var __cond = boundary - size - size ;
		if (__cond > 0) {
			{
				var __cond = boundary - align - align ;
				if (__cond > 0) {
					al = size ;
				}
			}
		}
	}
	bunch = (var)malloc((size + al));
	{
		var __cond = boundary ;
		if (__cond > 0) {
			start = bunch - ((var(*)())mod)(bunch, boundary);
			end = (bunch + size) - ((var(*)())mod)((bunch + size), boundary);
			{
				var __cond = end - start ;
				if (__cond != 0) {
					bunch = end;
				}
			}
		}
	}
	{
		var __cond = align ;
		if (__cond > 0) {
			bunch = bunch - ((var(*)())mod)(bunch, align);
		}
	}
	return bunch;
}
var clamp8(var value)
{
	{
		var __cond = value ;
		if (__cond >= 0) {
			return value % 256;
		}
	}
	return 255 - ((-value - 1) % 256);
}
var clamp16(var value)
{
	{
		var __cond = value ;
		if (__cond >= 0) {
			return value % 65536;
		}
	}
	return 65535 - ((-value - 1) % 65536);
}
var clamp24(var value)
{
	{
		var __cond = value ;
		if (__cond >= 0) {
			return value % 16777216;
		}
	}
	return 16777215 - ((-value - 1) % 16777216);
}
var rd8(var base, var offset)
{
	var  align;
	base = base + offset;
	align = base % 4;
	{
		var __cond = align ;
		if (__cond == -3) {
			align = 1;
		} else if (__cond == -2) {
			align = 2;
		} else if (__cond == -1) {
			align = 3;
		}
	}
	base = base - align;
	{
		var __cond = align ;
		if (__cond == 1) {
			return (((var(*)())clamp8)((((*((volatile var*)((volatile char*)(base)+(0))))) / 256)));
		} else if (__cond == 2) {
			return (((var(*)())clamp8)((((*((volatile var*)((volatile char*)(base)+(0))))) / 65536)));
		} else if (__cond == 3) {
			return (((var(*)())clamp8)((((*((volatile var*)((volatile char*)(base)+(0))))) / 16777216)));
		}
	}
	return ((var(*)())clamp8)((*((volatile var*)((volatile char*)(base)+(0)))));
}
var rd16(var base, var offset)
{
	var  align;
	base = base + offset;
	align = base % 4;
	{
		var __cond = align ;
		if (__cond == -3) {
			align = 1;
		} else if (__cond == -2) {
			align = 2;
		} else if (__cond == -1) {
			align = 3;
		}
	}
	base = base - align;
	{
		var __cond = align ;
		if (__cond == 1) {
			return ((var(*)())clamp16)(((*((volatile var*)((volatile char*)(base)+(0)))) / 256));
		} else if (__cond == 2) {
			return ((var(*)())clamp16)(((*((volatile var*)((volatile char*)(base)+(0)))) / 65536));
		} else if (__cond == 3) {
			return ((var(*)())rd8)(base, 3) + ((var(*)())rd8)(base, 4) * 256;
		}
	}
	return ((var(*)())clamp16)((*((volatile var*)((volatile char*)(base)+(0)))));
}
var rd32(var base, var offset)
{
	var  align;
	base = base + offset;
	{
		var __cond = align ;
		if (__cond == -3) {
			align = 1;
		} else if (__cond == -2) {
			align = 2;
		} else if (__cond == -1) {
			align = 3;
		}
	}
	align = base % 4;
	base = base - align;
	{
		var __cond = align ;
		if (__cond == 1) {
			return ((var(*)())clamp24)(((*((volatile var*)((volatile char*)(base)+(0)))) / 256)) + ((var(*)())rd8)(base, 4) * 16777216;
		} else if (__cond == 2) {
			return ((var(*)())rd16)(base, 2) + ((var(*)())rd16)(base, 4) * 65536;
		} else if (__cond == 3) {
			return ((var(*)())rd8)(base, 3) + ((var(*)())clamp24)((*((volatile var*)((volatile char*)(base)+(4))))) * 256;
		}
	}
	return (*((volatile var*)((volatile char*)(base)+(0))));
}
var wr32(var base, var offset, var value)
{
	*((volatile var*)((volatile unsigned char*)(base)+(offset))) = value;
	return 0;
}
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
	var tmp;
	var timeout;
};
var xhci_ctrl__init(var self, var base0, var base1, var irq, var size)
{
	struct xhci_ctrl *__self = (void*)self;(void)__self;
	var  v;
	struct xhci_root *root_hub;
	__self->tmp = (var)malloc(16);
	__self->base = base0;
	__self->cap = __self->base;
	__self->op = __self->base + ((var(*)())rd8)(__self->cap, CAPLENGTH);
	__self->rt = __self->base + ((var(*)())rd32)(__self->cap, RTSOFF) ;
	__self->db = __self->base + ((var(*)())rd32)(__self->cap, DBOFF) ;
	__self->hciversion = ((var(*)())rd16)(__self->cap, HCIVERSION);
	__self->xecp = __self->base + ((var(*)())rd16)(__self->cap, (HCCPARAMS1 + 2)) * 4;
	__self->ac64 = ((var(*)())rd8)(__self->cap, HCCPARAMS1) % 2;
	__self->csz = (((var(*)())rd8)(__self->cap, HCCPARAMS1) % 8) / 4;
	__self->max_slots = ((var(*)())rd8)(__self->cap, HCSPARAMS1);
	__self->ist = ((var(*)())rd8)(__self->cap, HCSPARAMS2) % 16 ;
	(*((var*)&root_hub)) = __self->root;
	((var(*)())xhci_root__init)((var)root_hub, self);
	((var(*)())xhci_ctrl__parse_extended)(self);
	((var(*)())xhci_ctrl__reset)(self);
	v = ((16777216 * 25) / 16777216) % 256;
	printf("%x",(((var(*)())clamp8)(v)));
	printf("%s","\nhello world\n");
	printf("%x",(__self->hciversion));
	printf("%s","\n");
	return 0;
}
var xhci_ctrl__parse_extended(var self)
{
	struct xhci_ctrl *__self = (void*)self;(void)__self;
	var  next;
	var  id, size, minor, major;
	var  cp_off, cp_count, psic, slot_type;
	struct xhci_root *rh;
	(*((var*)&rh)) = __self->root;
	next = __self->xecp;
	while (1) {
		var __cond = next ;
		if (__cond != 0) {
			id = ((var(*)())rd8)(next, 0);
			size = ((var(*)())rd8)(next, 1) * 4;
			{
				var __cond = size ;
				if (__cond == 0) {
					break;
				}
			}
			minor = ((var(*)())rd8)(next, 2);
			major = ((var(*)())rd8)(next, 3) ;
			{
				var __cond = id ;
				if (__cond == 1) {
					printf("%s","USB legacy\n");
				} else if (__cond == 2) {
					printf("%s","Supported protocol: ");
					((volatile unsigned char*)__self->tmp)[0] = ((var(*)())rd8)(next, 4);
					((volatile unsigned char*)__self->tmp)[1] = ((var(*)())rd8)(next, 5);
					((volatile unsigned char*)__self->tmp)[2] = ((var(*)())rd8)(next, 6);
					((volatile unsigned char*)__self->tmp)[3] = ((var(*)())rd8)(next, 7);
					((volatile unsigned char*)__self->tmp)[4] = 0;
					printf("%s",(char*)(__self->tmp));
					printf("%s","\n");
					cp_off = ((var(*)())rd8)(next, 8);
					cp_count = ((var(*)())rd8)(next, 9);
					psic = ((var(*)())rd8)(next, 11);
					slot_type = ((var(*)())rd8)(next, 12) % 32;
					((var(*)())xhci_root__set_ports_protocol)((var)rh, cp_off, cp_count, major);
					{
						var __cond = psic ;
						if (__cond == 0) {
						} else if (__cond != 0) {
						}
					}
				} else if (__cond == 3) {
				} else if (__cond == 4) {
				} else if (__cond == 5) {
				} else if (__cond == 6) {
				} else if (__cond == 10) {
				} else if (__cond == 17) {
				} else if (__cond == 255) {
				}
			}
			next = next + size ;
		} else { break; }
	}
	return 0;
}
var xhci_ctrl__sleep(var self)
{
	struct xhci_ctrl *__self = (void*)self;(void)__self;
	__self->timeout = __self->timeout - 1;
	return 0;
}
var xhci_ctrl__reset(var self)
{
	struct xhci_ctrl *__self = (void*)self;(void)__self;
	var  to, cnr;
	to = 100000;
	__self->timeout = 1000;
	while (1) {
		var __cond = __self->timeout ;
		if (__cond != 0) {
			cnr = ((var(*)())rd8)(__self->op, (USBSTS+1)) / 128;
			{
				var __cond = cnr ;
				if (__cond == 0) {
					break;
				}
#error "cond expected @ line 259 in ..\\src\\xhci_ctrl.3p"
