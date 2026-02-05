/*
 *                         OS-3o3 Operating System
 *
 *                      text output for the kernel
 *
 *                      13 may MMXXIV PUBLIC DOMAIN
 *           The authors disclaim copyright to this source code.
 *
 *
 */

#include "klib.h"

#include "font.h"
#define FONT_HEIGHT 16
#define FONT_WIDTH 8

k__u32 k__shell_fg = 0xFFFFFFFF;
k__u32 k__shell_bg = 0xFF000000;
k__i32 k__shell_x = 0;
k__i32 k__shell_y = 0;

void k__init_term()
{
#ifndef __EFI__
	serial__init();
#endif
	k__fb_init();
	k__shell_fg = 0xFFFFFF07;
	k__shell_bg = 0xFF000000;
	k__shell_x = 0;
	k__shell_y = 0;
}

void k__draw_char(k__int x, k__int y, k__u32 c)
{
	k__u8 *d;
	k__i32 i, j;
	k__u8 *p;
	k__u8 *pl;
	k__i32 l;

	if (c < ' ' || c > 126) {
		return;
	}
	c -= ' ';
	d = (k__u8 *)&font[16 * c];
	p = (k__u8 *)k__fb + (y * k__fb_pitch);
	if (k__fb_bpp == 32) {
		for (i = 0; i < 16; i++) {
			if ((y + i) < 0) {
				continue;
			} else if ((y + i) >= k__fb_height) {
				return;
			}
			l = d[i];
			pl = p + (x * 4);
			for (j = 0; j < 8; j++) {
				if ((x + j) < 0) {
				} else if ((x + j) >= k__fb_width) {
				} else if (l & 0x80) {
					((k__i32 *)pl)[0] = k__shell_fg;
				} else {
					((k__i32 *)pl)[0] = k__shell_bg;
				}
				pl += 4;
				l <<= 1;
			}
			p += k__fb_pitch;
		}
	} else if (k__fb_bpp == 8) {
		for (i = 0; i < 16; i++) {
			if ((y + i) < 0) {
				continue;
			} else if ((y + i) >= k__fb_height) {
				return;
			}
			l = d[i];
			pl = p + x;
			for (j = 0; j < 8; j++) {
				if ((x + j) < 0) {
				} else if ((x + j) >= k__fb_width) {
				} else if (l & 0x80) {
					pl[0] = k__shell_fg;
				} else {
					pl[0] = k__shell_bg;
					pl[0] = 0;
				}
				pl += 1;
				l <<= 1;
			}
			p += k__fb_pitch;
		}
	} else {
		for (i = 0; i < 16; i++) {
			if ((y + i) < 0) {
				continue;
			} else if ((y + i) >= k__fb_height) {
				return;
			}
			l = d[i];
			pl = p + (x * 3);
			for (j = 0; j < 8; j++) {
				if ((x + j) < 0) {
				} else if ((x + j) >= k__fb_width) {
				} else if (l & 0x80) {
					pl[0] = k__shell_fg;
					pl[1] = k__shell_fg >> 8;
					pl[2] = k__shell_fg >> 16;
				} else {
					pl[0] = k__shell_bg;
					pl[1] = k__shell_bg >> 8;
					pl[2] = k__shell_bg >> 16;
				}
				pl += 3;
				l <<= 1;
			}
			p += k__fb_pitch;
		}
	}
}

void k__scroll(k__int a)
{
	k__u8 *src;
	k__i32 i, j;
	k__u8 *p;
	if (a <= 0) {
		return;
	}

	src = (k__u8 *)k__fb + (a * k__fb_pitch);
	k__memcpy((void *)k__fb, src, (k__fb_height - a) * k__fb_pitch);

	for (i = 0; i < a; i++) {
		p = (k__u8 *)k__fb + ((k__fb_height - i - 1) * k__fb_pitch);
		if (k__fb_bpp == 32) {
			for (j = 0; j < k__fb_width; j++) {
				((k__i32 *)p)[0] = k__shell_bg;
				p += 4;
			}
		} else if (k__fb_bpp == 8) {
			for (j = 0; j < k__fb_width; j++) {
				p[0] = (utf8)k__shell_bg;
				p += 1;
			}
		} else {
			for (j = 0; j < k__fb_width; j++) {
				p[0] = k__shell_bg;
				p[1] = k__shell_bg >> 8;
				p[2] = k__shell_bg >> 16;
				p += 3;
			}
		}
	}
}

void k__putchar(k__i32 ch)
{
	k__i32 n = 0;
	if (ch == '\n') {
		k__shell_x = -FONT_WIDTH;
		k__shell_y += FONT_HEIGHT;
	} else if (ch == '\b') {
		k__shell_x -= FONT_WIDTH;
		if (k__shell_x < 0) {
			k__shell_x = 0;
		}
		k__draw_char(k__shell_x, k__shell_y, ' ');
		k__shell_x -= FONT_WIDTH;
	} else {
		k__draw_char(k__shell_x, k__shell_y, ch);
	}

	k__shell_x += FONT_WIDTH;
	if (k__shell_x >= k__fb_width) {
		k__shell_x = 0;
		k__shell_y += FONT_HEIGHT;
	}
	while (k__shell_y > (k__fb_height - FONT_HEIGHT)) {
		n++;
		k__shell_y -= FONT_HEIGHT;
	}
	if (n) {
		k__scroll(FONT_HEIGHT * n);
	}
}

#ifdef __EFI__
void uefi__puts(const char *str);
#endif

void k__puts(utf8 *s)
{
#ifdef __EFI__
	if (!k__fb) {
		uefi__puts(s);
		return;
	}
#endif
	while (*s) {
		k__putchar(*s);
#ifndef __EFI__
		if (*s == '\n') {
			serial__write('\r');
		}
		serial__write(*s);
#endif
		s++;
	}
}
