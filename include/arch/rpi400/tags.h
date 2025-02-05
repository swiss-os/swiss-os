
typedef struct property_tag
{
    os_uint32 tag_id;
    os_uint32 value_buf_size;
    os_uint32 value_length;
} property_tag;

typedef struct property_tag_simple
{
    property_tag tag;
    os_uint32 value;
} property_tag_simple;

typedef struct property_buffer
{
    os_uint32 buf_size;
    os_uint32 code;
    os_uint8 tags[0];
} property_buffer;

#define CODE_RESPONSE_SUCCESS 0x80000000
#define VALUE_LENGTH_RESPONSE (1 << 31)
#define PROPTAG_NOTIFY_XHCI_RESET 0x00030058
#define PROPTAG_END 0x00000000
#define CODE_REQUEST 0x00000000
#define MAILBOX_BASE (PERIPHERAL_BASE + 0xB880)
#define MAILBOX0_READ (MAILBOX_BASE + 0x00)
#define MAILBOX0_STATUS (MAILBOX_BASE + 0x18)
#define MAILBOX1_WRITE (MAILBOX_BASE + 0x20)
#define MAILBOX1_STATUS (MAILBOX_BASE + 0x38)
#define MAILBOX_STATUS_EMPTY 0x40000000
#define MAILBOX_STATUS_FULL 0x80000000
#define BCM_MAILBOX_PROP_OUT 8

os_intn tags__get_tags(void *tag, os_uint32 size);
