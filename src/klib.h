
/*
 *                         OS-3o3 Operating System
 *
 *                      12 may MMXXIV PUBLIC DOMAIN
 *           The authors disclaim copyright to this source code.
 *
 *
 */
#ifndef KLIB_H
#define KLIB_H

#ifndef VAR_TYPEDEF_
#define VAR_TYPEDEF_
#ifdef __AMD64__
typedef long long k__int;
typedef unsigned long long k__uint;
typedef unsigned long long k__u64;
#elif defined(__RPI400__)
typedef long k__int;
typedef unsigned long k__uint;
typedef unsigned long long k__u64;
#else
typedef long k__int;
typedef unsigned long k__uint;
typedef unsigned long long k__u64;
/*typedef struct {unsigned long low; unsigned long high;} k__u64;*/
#endif
#define NULL ((void *)0)
typedef unsigned char k__u8;
typedef unsigned long k__u32;
typedef unsigned short k__u16;
typedef int k__i32;
typedef char utf8;
typedef short utf16;
#endif

#define GFX__MODE_RGBA 1
#define GFX__MODE_BGRA 2
#define GFX__MODE_VGA16 3
#define GFX__MODE_VGA256 4
k__u32 gfx__get_mode(void);
k__u8 *gfx__swap_buffer(void);
k__u32 gfx__get_width(void);
k__u32 gfx__get_height(void);
k__u32 gfx__get_pitch(void);

void mmu__init();
k__int pci__init();
k__int xhci__pre_init();
void xhci__init();
int serial__init();
int serial__write(int c);

#define K__DEV_CALL                                                            \
	k__int (*open)(k__int dev, k__int filename, k__int flags, k__int mode);               \
	k__int (*read)(k__int fd, k__int buf, k__int count);                               \
	k__int (*write)(k__int fd, k__int buf, k__int count);                              \
	k__int (*close)(k__int fd);                                                  \
	k__int (*lseek)(k__int fd, k__int offset, k__int whence);                          \
	k__int (*ioctl)(k__int fd, k__int cmd, k__int arg);                                \
	k__int dev_name

#define K__DEV                                                                 \
	struct k__dev_call *call;                                              \
	k__int fd;                                                                \
	k__int offset

struct k__dev_call {
	K__DEV_CALL;
};

struct k__dev {
	K__DEV;
};

#define K__ALIGNED_HEAP_SIZE 0x10000
k__int k__interrupt_service_routine(k__int n, k__int data);

void *k__alloc_aligned(k__int size, k__int align, k__int boundary);
void k__printf(const utf8 *str, ...);
k__int k__sprintf(utf8 *buf, const utf8 *str, ...);
void *k__memcpy(void *dest, void *src, k__int n);
k__int k__memcmp(void *str1, void *str2, k__int n);
void *k__memset(void *str, k__int c, k__int n);
void k__usleep(k__int us);
k__int k__tick();
k__int k__thread_sleep(k__int ticks);
void k__init_term();
k__int k__init_vfs();
void k__draw_char(k__int x, k__int y, k__u32 c);
void k__puts(utf8 *s);
void k__scroll(k__int a);
k__int k__atoi(const utf8 *str);

k__int k__wr32(k__int base, k__int offset, k__int value);
k__int k__rd32(k__int base, k__int offset);
k__int k__rd16(k__int base, k__int offset);
k__int k__rd8(k__int base, k__int offset);
k__int k__aligned_alloc(k__int size, k__int align, k__int boundary);
k__int k__alloc(k__int size);
k__int k__free(k__int ptr);
k__int k__div(k__int a, k__int b);

k__int k__open(k__int filename, k__int flags, k__int mode);
k__int k__read(k__int fd, k__int buf, k__int count);
k__int k__write(k__int fd, k__int buf, k__int count);
k__int k__close(k__int fd);
k__int k__lseek(k__int fd, k__int offset, k__int whence);
k__int k__ioctl(k__int fd, k__int cmd, k__int arg);

k__int k__fill_read(k__int fd, k__int buf, k__int count, k__int data, k__int len);
k__int k__list_files(k__int filename);

k__int k__fd_new(k__int data);
k__int k__fd_get_data(k__int fd);
k__int k__fd_dispose(k__int fd);

#define k__read8(a)                                                            \
	(((*((volatile k__u32 *volatile)(((volatile k__int)(a) & ~3)))) >>        \
	  ((volatile k__int)(a)&3) * 8) &                                         \
	 0xFF)
#define k__write8(a, b) ((*((volatile k__u8 *volatile)(a)) = (b)))
#define k__read16(a)                                                           \
	((k__read8(((volatile k__u8 *volatile)(a)) + 1) << 8) | k__read8(a))
#define k__write16(a, b) ((*((volatile k__u32 *volatile)(a)) = (b)))
#define k__read32(a)                                                           \
	(((a)&3)                                                               \
	     ? ((k__read16(((volatile k__int *)(a)) + 2) << 16) | k__read16(a))   \
	     : (*((volatile k__u32 *)(a))))
#define k__write32(a, b) ((*((volatile k__u32 *volatile)(a)) = (b)))

void k__fb_init();
void fb_swap(void);
k__u32 *get_prog_end();
void syscall(k__u32);
void k__sfence(void);
k__int xhci__open(k__int n, k__int filename, k__int flags, k__int mode);
void xhci__irq(k__u32 n);
void interrupt__init();

#ifdef __I386__
#include "defs.h"
#define k__heap_alloc(a, b) rtos__heap_alloc(a, b)
#endif
#ifdef __AMD64__
#include "defs.h"
void acpi__init(void *);
void load_gdt();
void paging_on();
k__u64 paging_off();
#define k__heap_alloc(a, b) NULL
k__u8 in8(k__int port);
k__u32 in16(k__int port);
k__u8 in8(k__int port);
k__u32 in16(k__int port);
k__u32 in32(k__int port);
void out8(k__int port, k__int data);
void out16(k__int port, k__int data);
void out32(k__int port, k__int data);
#include "internal.h"
#endif
#ifdef __RPI400__
#include "arch/rpi400/defs.h"
#include "arch/rpi400/tags.h"
#define k__heap_alloc(a, b) rtos__heap_alloc(a, b)
void os__data_sync_barrier();
void os__data_mem_barrier();
#endif

#ifdef __I386__
k__u8 in8(k__int port);
k__u32 in16(k__int port);
k__u32 in32(k__int port);
void out8(k__int port, k__int data);
void out16(k__int port, k__int data);
void out32(k__int port, k__int data);
void beep();
#include "arch/i386/internal.h"
#endif /* __I386__ */

#ifdef __RPI400__
#include "arch/rpi400/internal.h"
k__int kbhit();
k__int getch();

#endif /*__RPI400__ */
#endif
