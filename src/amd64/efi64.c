
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


#include "../../include/efi.h"
#include "../../include/klib.h"
#include "../../include/pci.h"

extern char io32[];
#include "../../bin/io32.h"


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

/* GDT */
static void gdt_entry(os_uint32 offset,
                      os_uint16 limit15_0,
                      os_uint16 base15_0,
                      os_uint8 base23_16,
                      os_uint8 type,
                      os_uint8 limit19_16_and_flags,
                      os_uint8 base31_24)
{
    os_uint8 *g = (os_uint8 *)((void *)GDT_ADDR + offset);
    (*(os_uint16 *)&g[0]) = limit15_0;
    (*(os_uint16 *)&g[2]) = base15_0;
    g[4] = base23_16;
    g[5] = type;
    g[6] = limit19_16_and_flags;
    g[7] = base31_24;
}

/* GDTR */
static void gdtr()
{
    os_uint16 *g = (os_uint16 *)(GDTP_ADDR);
    g[0] = GDTP_ADDR - GDT_ADDR - 1;
    *((os_uint64 *)(g + 1)) = GDT_ADDR;
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
 * remap a page using the MMU
 */
os_uint32 remap(os_uint32 *page_directory, os_uint32 default_addr, os_uint64 src, os_uint32 len)
{
    os_intn i;
    len = len / 0x400000;
    if (len < 1)
    {
        len = 1;
    }
    for (i = default_addr / 0x400000; i < (default_addr / 0x400000 + len);
         i++)
    {
        os_uint32 bit39_32 = src >> 32; /* high address bits*/
        os_uint32 a = i - (default_addr / 0x400000) + ((src & 0xFFFFFFFF) / 0x400000);
        page_directory[i] = (bit39_32 << 13) | ((a) << 22) | 0x83; /* P RW US PS */
    }
    k__printf(" Remapped %x:%x -> %x len: %x\r\n", (src >> 32) & 0xFFFFFFFF,
              src & 0xFFFFFFFF, default_addr, len * 0x400000);
    return len;
}

/**
 * re-map XHCI address range if needed
 */
void remap_xhci_ctrl(os_uint32 *page_directory, os_intn num, os_uint32 low, os_uint32 high, os_uint32 size)
{
    os_uint32 da = XHCI_DEFAULT_BASE0;
    if (high == 0)
    {
        return;
    }
    if (num == 1)
    {
        da = XHCI_DEFAULT_BASE1;
    }
    remap(page_directory, da, (((os_uint64)high) << 32) + low, size);
}

/**
 * remap the PCI XHCI address using MMU pageing
 */
os_intn remap_xhci(os_uint32 *page_directory)
{
    os_uint32 b = 0;
    os_uint32 s = 0;
    os_uint32 f = 0;
    os_uint32 vendor;
    os_uint32 class_, subc, pif;
    os_intn i = 0;
    os_uint32 *bus = &b;
    os_uint32 *slot = &s;
    os_uint32 *function = &f;
    os_uint32 high, low, size;

    for (*bus = 0; *bus < PCI_MAX_BUS; (*bus)++)
    {
        for (*slot = 0; *slot < PCI_MAX_SLOT; (*slot)++)
        {
            for (*function = 0; *function < PCI_MAX_FUNCTION; (*function)++)
            {
                vendor = pci__cfg_read_vendor(*bus, *slot, *function);
                if (vendor != PCI_VENDOR_NO_DEVICE)
                {
                    /*device = pci__cfg_read_device(*bus, *slot, *function);*/
                    class_ = pci__cfg_read_class(*bus, *slot, *function);
                    subc = pci__cfg_read_subclass(*bus, *slot, *function);
                    pif = pci__cfg_read_prog_if(*bus, *slot, *function);

                    if (class_ == PCI_CLASS_SERIAL_BUS_CONTROLLER && subc == PCI_SUBCLASS_USB && pif == PCI_PROG_IF_XHCI_CONTROLLER)
                    {
                        if (i >= XHCI_MAX_CONTROLLER)
                        {
                            k__printf("Too many xHCI controller found\n");
                        }
                        else
                        {
                            low = pci__cfg_read_base_addr(*bus, *slot, *function, 0);
                            high = pci__cfg_read_base_addr(*bus, *slot, *function, 1);
                            size = pci__cfg_write_base_addr(*bus, *slot, *function, 0, 0xFFFFFFFF) & ~0xF;
                            pci__cfg_write_base_addr(*bus, *slot, *function, 0, low);
                            size = (~size) + 1;
                            remap_xhci_ctrl(page_directory, i, low & ~0x0F, high, size);
                        }
                        i++;
                    }
                }
            }
        }
    }
    return 0;
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
    efi_configuration_table *ct;
    efi_guid ac = efi_acpi_20_table_guid;
    os_uint32 *page_directory;

    ImageHandle = ih;
    SystemTable = st;

    if (st->ConOut) {
        st->ConOut->Reset(st->ConOut, 0);
        echo = st->ConOut->OutputString;
    }
    ConOut = st->ConOut;
    k__printf(" OS-3o3 Image: %x", ImageHandle);

    
    SystemTable->BootServices->LocateProtocol(&guid, (void *)0, (void **)&gfx);
    k__printf(" GFX: %x\r\n", gfx);

    /* copy the 32bit kernel to is location */
    os_uint64 *dst = (os_uint64 *)(KERNEL_ENTRY32);
    i = 0;
    while (i < sizeof(io32))
    {
        *dst = *((os_uint64 *)(&io32[i]));
        dst++;
        i += 8;
    }

    /* ACPI parsing */
    ct = st->ConfigurationTable;
    i = st->NumberOfTableEntries;
    while (i > 0)
    {
        if (!k__memcmp(&ct->VendorGuid, &ac, sizeof(efi_guid)))
        {
            acpi__init(ct->VendorTable);
        }
        ct++;
        i--;
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

    fbaddr = (os_uint64)gfx->Mode->FrameBufferBase;
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

    /*https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html
      4.3 32-BIT PAGING sdm-vol-3abcd.pdf
    */
    page_directory = (os_uint32 *)(PDT_ADDR);
    for (i = 0; i < 1024; i++)
    {
        page_directory[i] = ((i) << 22) | 0x83;
    }
    if (fbaddr > 0xFFFFFFFFULL)
    {
        *((os_uint32*)MEM_GFX_FB) = FB_DEFAULT_ADDR;
        remap(page_directory, FB_DEFAULT_ADDR, fbaddr, 64 * 0x400000);
    }
    k__printf(" GFX: %x", (long long)gfx->Mode->Mode);
    k__printf("%x", (long long)mode);
    k__printf(" Frame buffer: ");
    k__printf("%x", (long long)gfx->Mode->FrameBufferBase);
    k__printf(" --- ");
    k__printf("%x", (long long)gfx->Mode->FrameBufferSize);

    remap_xhci(page_directory);

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


    /*  https://wiki.osdev.org/GDT_Tutorial
      https://blog.llandsmeer.com/tech/2019/07/21/uefi-x64-userland.html */
    gdt_entry(0x00, 0, 0, 0, 0x00, 0x00, 0); /* null */
    gdt_entry(0x08,
              0xFFFF,
              0,
              0,
              0x9A,
              0xCF,
              0);                                 /* 32 bit compatibility mode code segment */
    gdt_entry(0x10, 0xFFFF, 0, 0, 0x92, 0xCF, 0); /* 32 bit data segment */
    gdt_entry(0x18, 0xFFFF, 0, 0, 0x9A, 0xAF, 0); /* code segment */
    gdt_entry(0x20, 0xFFFF, 0, 0, 0x92, 0xAF, 0); /* data segment */ 
    gdtr();
  

    /* jump to 32bit mode */
    load_gdt();

    /* should never been here */
    while (1)
    {
        ;
    }
    return -1;
}
