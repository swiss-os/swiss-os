
/*
 *                       OS-3o3 Operating System
 *
 *                        AMD64 UEFI boot loader.
 *                  It switches the CPU to 32 bit mode,
 *                    then it calls the 32 bit kernel
 *
 *                      13 may MMXXIV PUBLIC DOMAIN
 *           The authors disclaim copyright to this source code.
 *
 *
 * https://uefi.org/specs/UEFI/2.10/12_Protocols_Console_Support.html#efi-graphics-output-protocol
 * https://uefi.org/specs/UEFI/2.10/13_Protocols_Media_Access.html
 * https://stackoverflow.com/questions/22962251/how-to-enter-64-bit-mode-on-a-x86-64
 *
 */

#include "efi.h"
#include "klib.h"

/*
extern char io32[];
#include "../../bin/io32.h"
*/
char io32[100];

void efios__main(void);

void *ImageHandle;
efi_system_table *SystemTable;
efi_simple_text_output_protocol *ConOut;
efi_graphics_output_protocol *gfx;
efi_text_string echo;
static k__u8 MemoryMap[UEFI_MMAP_SIZE];
volatile k__u8 *k__fb = 0;
k__u16 k__fb_height;
k__u16 k__fb_width;
k__u16 k__fb_pitch;
k__u16 k__fb_bpp;

void k__fb_init() { /*k__fb = 0;*/ }

/**
 * write string to UEFI text console
 */
void uefi__puts(utf8 *str)
{
	utf16 buf[256];
	k__int i = 0;
	k__int j = 0;
	if (!ConOut) {
		return;
	}
	while (str[i] && i < 254) {
		if (str[i] == '\n') {
			buf[j] = '\r';
			j++;
		}
		buf[j] = (k__u8)str[i];
		i++;
		j++;
	}
	buf[j] = 0;
	echo(ConOut, buf);
}

/*
 * UEFI main loader function
 */
int efi_main(void *ih, efi_system_table *st)
{
	k__int MemoryMapSize = UEFI_MMAP_SIZE;
	k__int MapKey, DescriptorSize;
	k__u32 DescriptorVersion;
	efi_graphics_output_mode_information *info =
	    (efi_graphics_output_mode_information *)(void *)0;
	k__int size;
	k__u32 i;
	k__u32 mode = 0xFFFFF;
	efi_guid guid = efi_graphic_output_protocol_guid;
	efi_status r;
	k__u64 fbaddr;
/*
	efi_configuration_table *ct;
	k__u32 *page_directory;
*/
	ImageHandle = ih;
	SystemTable = st;

	if (st->ConOut) {
		st->ConOut->Reset(st->ConOut, 0);
		echo = st->ConOut->OutputString;
	}
	ConOut = st->ConOut;
	k__printf(" OS-3o3 Image: %x", ImageHandle);


	SystemTable->BootServices->LocateProtocol(&guid, (void *)0,
						  (void **)&gfx);
	k__printf(" GFX: %x  ", gfx);
	

	k__printf("JJJJ %x %d %d %d\n", KERNEL_ENTRY32/* +0x11c70*/,
		0,0,0);

	/* try to find graphic framebuffer 640x480 32bpp */
	i = 0;
	while (!gfx->QueryMode(gfx, i, &size, &info)) {
		if (info->PixelFormat ==
			PixelRedGreenBlueReserved8BitPerColor ||
		    info->PixelFormat ==
			PixelBlueGreenRedReserved8BitPerColor) {
			k__printf("%d x %d ", info->HorizontalResolution,
				  info->VerticalResolution);
			switch (info->PixelFormat) {
			case PixelRedGreenBlueReserved8BitPerColor: /* k__u8[0] =
								       Red */
				k__printf(" RGBA");
				break;
			case PixelBlueGreenRedReserved8BitPerColor: /* k__u8[0] =
								       Blue */
				k__printf(" BGRA");
				break;
			default:
				k__printf("%x", (long long)info->PixelFormat);
			}
			if (info->PixelsPerScanLine != 640) {
				k__printf(" pxl/scan %d",
					  (long long)info->PixelsPerScanLine);
			}
			k__printf("\r\n");
			if (mode == 0xFFFFF) {
				mode = i;
			} else if (info->HorizontalResolution == 640 &&
				   info->VerticalResolution == 480) {
				mode = i;
				break;
			}
		}
		i++;
	}

	if (mode == 0xFFFFF) {
		k__printf("No compatible graphic mode found\r\n");
		while (1) {
			;
		}
	} else {
		/* switch to GOP */
		if (gfx->SetMode(gfx, mode)) {
			k__printf("Error switching graphic mode\r\n");
			while (1) {
				;
			}
		}
	}

	fbaddr = (k__u64)gfx->Mode->FrameBufferBase;
	k__fb = (k__u8 *)fbaddr;
	if (info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor) {
		*((k__u32 *)MEM_GFX_MODE) = GFX__MODE_RGBA;
	} else {
		*((k__u32 *)MEM_GFX_MODE) = GFX__MODE_BGRA;
	}
	k__fb_bpp = 32;
	k__fb_pitch = *((k__u32 *)MEM_GFX_PITCH) =
	    (k__u32)gfx->Mode->Info->PixelsPerScanLine * 4;
	k__fb_width = *((k__u32 *)MEM_GFX_WIDTH) =
	    (k__u32)gfx->Mode->Info->HorizontalResolution;
	k__fb_height = *((k__u32 *)MEM_GFX_HEIGHT) =
	    (k__u32)gfx->Mode->Info->VerticalResolution;
	*((k__u32 *)MEM_GFX_FB) = (k__u32)(fbaddr & 0xFFFFFFFF);


	efios__main();

	/* Quit UEFI services */
	r = efi_success;
	st->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey,
				       &DescriptorSize, &DescriptorVersion);
	r = st->BootServices->ExitBootServices(ImageHandle, MapKey);
	if (r != efi_success) {
		k__printf("\r\n Panic UEFI failure code ");
		k__printf("%x", r);
		k__printf("\r\n");
		while (1) {
			;
		}
	}


	/* should never been here */
	while (1) {
		;
	}
	return -1;
}
