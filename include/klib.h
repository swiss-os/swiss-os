
/*
 *                         OS-3o3 Operating System
 *
 *                      12 may MMXXIV PUBLIC DOMAIN
 *           The authors disclaim copyright to this source code.
 *
 *
 */
#include "os3.h"

#ifndef VAR_TYPEDEF_
#define VAR_TYPEDEF_
typedef os_intn var;
typedef os_uint32 k__u32;
typedef os_uint16 k__u16;
typedef os_uint8 k__u8;
#endif

void uefi__puts(const char *str);

#define K__ALIGNED_HEAP_SIZE 0x10000

void* k__alloc_aligned(os_size size, os_size align, os_size boundary);
void k__printf(const os_utf8* str, ...);
void* k__memcpy(void* dest, void* src, os_size n);
os_intn k__memcmp(void* str1, void* str2, os_size n);
void* k__memset(void* str, os_intn c, os_size n);
void k__usleep(os_intn us);
void k__init();
void k__draw_char(os_intn x, os_intn y, os_uint32 c);
void k__puts(os_utf8* s);
void k__scroll(os_intn a);

var k__wr32(var base, var offset, var value);
var k__rd32(var base, var offset);
var k__rd16(var base, var offset);
var k__rd8(var base, var offset);
var k__aligned_alloc(var size, var align, var boundary);
var k__alloc(var size);
var k__free(var ptr);
var k__div(var a, var b);

#define k__read8(a) (((*((volatile os_uint32*volatile)(((volatile os_intn)(a) & ~3)))) >> ((volatile os_intn)(a) & 3)*8) & 0xFF)
#define k__write8(a, b) ((*((volatile os_uint8*volatile)(a)) = (b)))
#define k__read16(a) ((k__read8(((volatile os_uint8 *volatile)(a)) + 1) << 8) | k__read8(a))
#define k__write16(a, b) ((*((volatile os_uint16*volatile)(a)) = (b)))
#define k__read32(a) (((a)&3) ? ((k__read16(((volatile os_intn*)(a)) + 2) << 16) | k__read16(a)) : (*((volatile os_uint32*)(a))))
#define k__write32(a, b) ((*((volatile os_uint32*volatile)(a)) = (b)))

void k__fb_init();
void fb_swap(void);
os_uint32* get_prog_end();
void syscall(os_uint32);
void k__sfence(void);

#ifdef __I386__
#include "arch/i386/defs.h"
#define k__heap_alloc(a, b) rtos__heap_alloc(a, b)
#endif
#ifdef __AMD64__
#include "arch/i386/defs.h"
void acpi__init(void*);
void load_gdt();
void paging_on();
os_uint64 paging_off();
#define k__heap_alloc(a, b) NULL
os_uint8 in8(os_intn port);
os_uint16 in16(os_intn port);
os_uint8 in8(os_intn port);
os_uint16 in16(os_intn port);
os_uint32 in32(os_intn port);
void out8(os_intn port, os_intn data);
void out16(os_intn port, os_intn data);
void out32(os_intn port, os_intn data);
#include "arch/amd64/internal.h"
#endif
#ifdef __RPI400__
#include "arch/rpi400/defs.h"
#include "arch/rpi400/tags.h"
#define k__heap_alloc(a, b) rtos__heap_alloc(a, b)
void os__data_sync_barrier();
void os__data_mem_barrier();
#endif

#ifdef __I386__
os_uint8 in8(os_intn port);
os_uint16 in16(os_intn port);
os_uint32 in32(os_intn port);
void out8(os_intn port, os_intn data);
void out16(os_intn port, os_intn data);
void out32(os_intn port, os_intn data);
void beep();
#include "arch/i386/internal.h"
#endif /* __I386__ */

#ifdef __RPI400__
#include "arch/rpi400/internal.h"
os_intn kbhit();
os_intn getch();

#endif /*__RPI400__ */
