#include "../include/xhci.h"
#include "../include/usb.h"

var usb__print_device_descriptor(struct xhci *self, var slot_id, var data)
{
	k__printf("DEsc 0x%x\n", data);
	return 0;
}

