
/*
 *   This software is dedicated to the public domain.
 */

#ifndef __EFI_H__
#define __EFI_H__ 1

#include "os3.h"

#define efi_handle void *
#define efi_status int
#define efi_physical_address void *
#define efi_success 0

typedef struct
{
    os_uint32 Data1;
    os_uint16 Data2;
    os_uint16 Data3;
    os_uint8 Data4[8];
} efi_guid;

#define efi_graphic_output_protocol_guid                   \
    {                                                      \
        0x9042a9de, 0x23dc, 0x4a38,                        \
        {                                                  \
            0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a \
        }                                                  \
    }

#define efi_acpi_20_table_guid                             \
    {                                                      \
        0x8868e871, 0xe4f1, 0x11d3,                        \
        {                                                  \
            0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81 \
        }                                                  \
    }

typedef struct efi_graphics_output_protocol efi_graphics_output_protocol;

typedef enum
{
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} efi_graphics_pixel_format;

typedef struct
{
    os_uint32 RedMask;
    os_uint32 GreenMask;
    os_uint32 BlueMask;
    os_uint32 ReservedMask;
} efi_pixel_bitmask;

typedef struct
{
    os_uint32 Version;
    os_uint32 HorizontalResolution;
    os_uint32 VerticalResolution;
    efi_graphics_pixel_format PixelFormat;
    efi_pixel_bitmask PixelInformation;
    os_uint32 PixelsPerScanLine;
} efi_graphics_output_mode_information;

typedef struct
{
    os_uint32 MaxMode;
    os_uint32 Mode;
    efi_graphics_output_mode_information *Info;
    os_intn SizeOfInfo;
    efi_physical_address FrameBufferBase;
    os_intn FrameBufferSize;
} efi_graphics_output_protocol_mode;

typedef efi_status (*efi_graphics_output_protocol_query_mode)(
    efi_graphics_output_protocol *This,
    os_uint32 ModeNumber,
    os_intn *SizeOfInfo,
    efi_graphics_output_mode_information **Info);

typedef efi_status (*efi_graphics_output_protocol_set_mode)(
    efi_graphics_output_protocol *This,
    os_uint32 ModeNumber);

typedef struct
{
    os_uint8 Blue;
    os_uint8 Green;
    os_uint8 Red;
    os_uint8 Reserved;
} efi_graphics_output_blt_pixel;

typedef enum
{
    EfiBltVideoFill,
    EfiBltVideoToBltBuffer,
    EfiBltBufferToVideo,
    EfiBltVideoToVideo,
    EfiGraphicsOutputBltOperationMax
} efi_graphics_output_blt_operation;

typedef efi_status (*efi_graphics_output_protocol_blt)(
    efi_graphics_output_protocol *This,
    efi_graphics_output_blt_pixel *BltBuffer,
    efi_graphics_output_blt_operation BltOperation,
    os_intn SourceX,
    os_intn SourceY,
    os_intn DestinationX,
    os_intn DestinationY,
    os_intn Width,
    os_intn Height,
    os_intn Delta);

typedef struct efi_graphics_output_protocol
{
    efi_graphics_output_protocol_query_mode QueryMode;
    efi_graphics_output_protocol_set_mode SetMode;
    efi_graphics_output_protocol_blt Blt;
    efi_graphics_output_protocol_mode *Mode;
} efi_graphics_output_protocol;

#define efi_system_table_signature 0x5453595320494249

typedef struct
{
    os_uint64 Signature;
    os_uint32 Revision;
    os_uint32 HeaderSize;
    os_uint32 CRC32;
    os_uint32 Reserved;
} efi_table_header;

typedef efi_status (*efi_locate_protocol)(efi_guid *Protocol,
                                          void *Registration,
                                          void **Interface);

typedef efi_status (*efi_exit_boot_services)(efi_handle ImageHandle,
                                             os_intn MapKey);

#define efi_raise_tpl void *
#define efi_restore_tpl void *
#define efi_allocate_pages void *
#define efi_free_pages void *

#define UEFI_MMAP_SIZE 0x4000
#define efi_memory_descriptor os_uint8
typedef efi_status (*efi_get_memory_map)(os_intn *MemoryMapSize,
                                         efi_memory_descriptor *MemoryMap,
                                         os_intn *MapKey,
                                         os_intn *DescriptorSize,
                                         os_uint32 *DescriptorVersion);

#define efi_allocate_pool void *
#define efi_free_pool void *
#define efi_create_event void *
#define efi_set_timer void *
#define efi_wait_for_event void *
#define efi_signal_event void *
#define efi_close_event void *
#define efi_check_event void *
#define efi_install_protocol_interface void *
#define efi_reinstall_protocol_interface void *
#define efi_uninstall_protocol_interface void *
#define efi_handle_protocol void *
#define efi_register_protocol_notify void *
#define efi_locate_handle void *
#define efi_locate_device_path void *
#define efi_install_configuration_table void *
#define efi_image_load void *
#define efi_image_start void *
#define efi_exit void *
#define efi_image_unload void *
#define efi_get_next_monotonic_count void *
#define efi_stall void *
#define efi_set_watchdog_timer void *
#define efi_connect_controller void *
#define efi_disconnect_controller void *
#define efi_open_protocol void *
#define efi_close_protocol void *
#define efi_open_protocol_information void *
#define efi_protocols_per_handle void *
#define efi_locate_handle_buffer void *
#define efi_install_multiple_protocol_interfaces void *
#define efi_uninstall_multiple_protocol_interfaces void *
#define efi_calculate_crc32 void *
#define efi_copy_mem void *
#define efi_set_mem void *
#define efi_create_event_ex void *

typedef struct
{
    efi_table_header Hdr;

    efi_raise_tpl RaiseTPL;
    efi_restore_tpl RestoreTPL;

    efi_allocate_pages AllocatePages;
    efi_free_pages FreePages;
    efi_get_memory_map GetMemoryMap;
    efi_allocate_pool AllocatePool;
    efi_free_pool FreePool;

    efi_create_event CreateEvent;
    efi_set_timer SetTimer;
    efi_wait_for_event WaitForEvent;
    efi_signal_event SignalEvent;
    efi_close_event CloseEvent;
    efi_check_event CheckEvent;

    efi_install_protocol_interface InstallProtocolInterface;
    efi_reinstall_protocol_interface ReinstallProtocolInterface;
    efi_uninstall_protocol_interface UninstallProtocolInterface;
    efi_handle_protocol HandleProtocol;
    void *Reserved;
    efi_register_protocol_notify RegisterProtocolNotify;
    efi_locate_handle LocateHandle;
    efi_locate_device_path LocateDevicePath;
    efi_install_configuration_table InstallConfigurationTable;

    efi_image_load LoadImage;
    efi_image_start StartImage;
    efi_exit Exit;
    efi_image_unload UnloadImage;
    efi_exit_boot_services ExitBootServices;

    efi_get_next_monotonic_count GetNextMonotonicCount;
    efi_stall Stall;
    efi_set_watchdog_timer SetWatchdogTimer;

    efi_connect_controller ConnectController;
    efi_disconnect_controller DisconnectController;

    efi_open_protocol OpenProtocol;
    efi_close_protocol CloseProtocol;
    efi_open_protocol_information OpenProtocolInformation;

    efi_protocols_per_handle ProtocolsPerHandle;
    efi_locate_handle_buffer LocateHandleBuffer;
    efi_locate_protocol LocateProtocol;
    efi_install_multiple_protocol_interfaces InstallMultipleProtocolInterfaces;
    efi_uninstall_multiple_protocol_interfaces
        UninstallMultipleProtocolInterfaces;

    efi_calculate_crc32 CalculateCrc32;

    efi_copy_mem CopyMem;
    efi_set_mem SetMem;
    efi_create_event_ex CreateEventEx;
} efi_boot_services;

typedef struct _efi_simple_text_output_protocol efi_simple_text_output_protocol;

typedef efi_status (*efi_text_string)(efi_simple_text_output_protocol *This,
                                      os_utf16 *String);
                                      
typedef efi_status (*efi_text_reset)(efi_simple_text_output_protocol *This,
                                      os_intn flag);

#define efi_text_test_string void *
#define efi_text_query_mode void *
#define efi_text_set_mode void *
#define efi_text_set_attribute void *
#define efi_text_clear_screen void *
#define efi_text_set_cursor_position void *
#define efi_text_enable_cursor void *
#define simple_text_output_mode void *

typedef struct _efi_simple_text_output_protocol
{
    efi_text_reset Reset;
    efi_text_string OutputString;
    efi_text_test_string TestString;
    efi_text_query_mode QueryMode;
    efi_text_set_mode SetMode;
    efi_text_set_attribute SetAttribute;
    efi_text_clear_screen ClearScreen;
    efi_text_set_cursor_position SetCursorPosition;
    efi_text_enable_cursor EnableCursor;
    simple_text_output_mode *Mode;
} efi_simple_text_output_protocol;

#define efi_simple_text_input_protocol void *
#define efi_runtime_services void *
typedef struct
{
    efi_guid VendorGuid;
    void *VendorTable;
} efi_configuration_table;

efi_graphics_output_protocol *gfx;

typedef struct
{
    efi_table_header Hdr;
    os_utf16 *FirmwareVendor;
    os_uint32 FirmwareRevision;
    efi_handle ConsoleInHandle;
    efi_simple_text_input_protocol *ConIn;
    efi_handle ConsoleOutHandle;
    efi_simple_text_output_protocol *ConOut;
    efi_handle StandardErrorHandle;
    efi_simple_text_output_protocol *StdErr;
    efi_runtime_services *RuntimeServices;
    efi_boot_services *BootServices;
    os_intn NumberOfTableEntries;
    efi_configuration_table *ConfigurationTable;
} efi_system_table;


#endif /*__EFI_H__*/
