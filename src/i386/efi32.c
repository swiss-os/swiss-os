
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

#include "../../include/klib.h"
#include "../../include/pci.h"
#include "../../include/efi.h"


extern char io32[];
#include "../../bin/io32.h"

void load_gdt(void);

void *ImageHandle;
efi_system_table *SystemTable;
efi_simple_text_output_protocol *ConOut;
efi_text_string echo;
static os_uint8 MemoryMap[UEFI_MMAP_SIZE];
volatile os_uint8 *k__fb = 0;
os_uint16 k__fb_height;
os_uint16 k__fb_width;
os_uint16 k__fb_pitch;
os_uint16 k__fb_bpp;


void k__fb_init()
{
    k__fb = 0;
}


/**
 * write string to UEFI text console
 */
void uefi__puts(const char *str)
{
    os_uint16 buf[256];
    os_intn i = 0;
    os_intn j = 0;
    if (!ConOut) {
        return;
    }
    while (str[i] && i < 254)
    {
        if (str[i] == '\n')
        {
            buf[j] = '\r';
            j++;
        }
        buf[j] = (os_uint8)str[i];
        i++;
        j++;
    }
    buf[j] = 0;
    echo(ConOut, buf);
}

/**
 * UEFI main loader function
 */
int efi_main(void *ih, efi_system_table *st)
{
    os_intn MemoryMapSize = UEFI_MMAP_SIZE;
    os_intn MapKey, DescriptorSize;
    os_uint32 DescriptorVersion;
    efi_graphics_output_mode_information *info = (efi_graphics_output_mode_information *)(void *)0;
    os_intn size;
    os_uint32 i;
    os_uint32 mode = 0xFFFFF;
    efi_guid guid = efi_graphic_output_protocol_guid;
    efi_status r;
    os_uint64 fbaddr;

    ImageHandle = ih;
    SystemTable = st;

    if (st->ConOut) {
        echo = st->ConOut->OutputString;
    }
    ConOut = st->ConOut;

    k__printf("OS-3o3 32Bit UEFI --\r\n");

    

    k__printf(" OS-3o3 32Bit Image: %x", ImageHandle);

    SystemTable->BootServices->LocateProtocol(&guid, (void *)0, (void **)&gfx);
    k__printf(" GFX: %x\r\n", gfx);

    /* copy the 32bit kernel to is location */
    os_uint32 *dst = (os_uint32 *)(KERNEL_ENTRY32);
    i = 0;
    while (i < sizeof(io32))
    {
        *dst = *((os_uint32 *)(&io32[i]));
        dst++;
        i += 4;
    }


    /* try to find graphic framebuffer 640x480 32bpp */
    i = 0;
    while (!gfx->QueryMode(gfx, i, &size, &info))
    {
        if (info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor || info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor)
        {
            k__printf("%d x %d ", info->HorizontalResolution,
                      info->VerticalResolution);
            switch (info->PixelFormat)
            {
            case PixelRedGreenBlueReserved8BitPerColor: /* byte[0] = Red */
                k__printf(" RGBA");
                break;
            case PixelBlueGreenRedReserved8BitPerColor: /* byte[0] = Blue */
                k__printf(" BGRA");
                break;
            default:
                k__printf("%x", (long long)info->PixelFormat);
            }
            if (info->PixelsPerScanLine != 640)
            {
                k__printf(" pxl/scan %d", (long long)info->PixelsPerScanLine);
            }
            k__printf("\r\n");
            if (mode == 0xFFFFF)
            {
                mode = i;
            }
            else if (info->HorizontalResolution == 640 && info->VerticalResolution == 480)
            {
                mode = i;
                break;
            }
        }
        i++;
    }
 

    if (mode == 0xFFFFF)
    {
        k__printf("No compatible graphic mode found\r\n");
        while (1)
        {
            ;
        }
    }
    else
    {
        k__printf("Graphic mode found: \r\n");
        /* switch to GOP */
        if (gfx->SetMode(gfx, mode))
        {
            k__printf("Error switching graphic mode\r\n");
            while (1)
            {
                ;
            }
        }
    }
    

    fbaddr = (os_uint32)gfx->Mode->FrameBufferBase;
    k__fb = (os_uint8 *)fbaddr;
    if (info->PixelFormat == PixelRedGreenBlueReserved8BitPerColor) {
        *((os_uint32*)MEM_GFX_MODE) = GFX__MODE_RGBA;
    } else {
        *((os_uint32*)MEM_GFX_MODE) = GFX__MODE_BGRA;
    }
    *((os_uint32*)MEM_GFX_PITCH) = (os_uint32) gfx->Mode->Info->PixelsPerScanLine * 4;
    *((os_uint32*)MEM_GFX_WIDTH) = (os_uint32) gfx->Mode->Info->HorizontalResolution;
    *((os_uint32*)MEM_GFX_HEIGHT) = (os_uint32) gfx->Mode->Info->VerticalResolution;
    *((os_uint32*)MEM_GFX_FB) = (os_uint32) (fbaddr & 0xFFFFFFFF);


    k__printf(" GFX: %x", (long long)gfx->Mode->Mode);
   
    k__printf("%x", (long long)mode);
    k__printf(" Frame buffer: ");
    k__printf("%x", (long long)gfx->Mode->FrameBufferBase);
    k__printf(" --- ");
    k__printf("%x", (long long)gfx->Mode->FrameBufferSize);

    /* Quit UEFI services */
    r = efi_success;
    st->BootServices->GetMemoryMap(
        &MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    r = st->BootServices->ExitBootServices(ImageHandle, MapKey);
    if (r != efi_success)
    {
        k__printf("\r\n Panic UEFI failure code ");
        k__printf("%x", r);
        k__printf("\r\n");
        while (1)
        {
            ;
        }
    }
    
    /* jump to 32bit kernel */
    load_gdt();

    /* should never been here */
    while (1)
    {
        ;
    }
    return -1;
}
