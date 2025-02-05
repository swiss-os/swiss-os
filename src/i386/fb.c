/******************************************************************************
 *                        OS-3o3 operating system
 *
 *                          framebuffer for i386
 *
 *            20 June MMXXIV PUBLIC DOMAIN by Jean-Marc Lienher
 *
 *        The authors disclaim copyright and patents to this software.
 *
 *****************************************************************************/

/* https://forum.osdev.org/viewtopic.php?f=2&t=30186 */

#include "../../include/klib.h"

volatile os_uint8 *k__fb;
os_uint8 *fb_front;
os_uint16 k__fb_height;
os_uint16 k__fb_width;
os_uint16 k__fb_pitch;
os_uint16 k__fb_bpp;
os_uint16 k__fb_isrgb;

void k__fb_init()
{
    k__fb_isrgb = 0;
    switch (*((os_uint32*)MEM_GFX_MODE)) {
    case GFX__MODE_BGRA:
        k__fb_bpp = 32;
        k__fb_isrgb = 1;
        break;
    case GFX__MODE_RGBA:
        k__fb_bpp = 32;
        k__fb_isrgb = 2;
        break;
    case GFX__MODE_VGA16:
        k__fb_bpp = 4;
        break;
    case GFX__MODE_VGA256:
        k__fb_bpp = 8;
        break;
    default:
        k__fb_bpp = 8;
    }
    k__fb_pitch = *((os_uint32*)MEM_GFX_PITCH);
    k__fb_width = *((os_uint32*)MEM_GFX_WIDTH);
    k__fb_height = *((os_uint32*)MEM_GFX_HEIGHT);
    fb_front = *((os_uint8**)MEM_GFX_FB);
    k__fb = fb_front;
}

void fb_swap()
{
}
