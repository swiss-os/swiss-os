
#define USB_DEVICE_DESCRIPTOR_SIZE 18 /* 9.6.1 */ 
#define USB_DEVICE_DESCRIPTOR_TYPE 1 /* USB 3.2 Chapter 9.3.1 Table 9-6. */

#define USB_GET_STANDARD_DEVICE_REQUEST 0x80 /* 9.3 */
#define USB_SET_STANDARD_DEVICE_REQUEST 0x00

var usb__print_device_descriptor(struct xhci *self, var slot_id, var data);

