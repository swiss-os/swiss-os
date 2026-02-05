
/*
 *   This software is dedicated to the public domain.
 */

#ifndef __EFI_H__
#define __EFI_H__ 1

#include "klib.h"

/* Bitfields are ordered such that bit 0 is the least significant bit. */
#define efi_bool k__u8
#define efi_handle void *
#define efi_status int
#define efi_physical_address void *
#define efi_success 0
#define efi_event void *
#define efi_lba k__u64
#define efi_tpl k__uint

typedef struct {
	k__u32 Data1;
	k__u16 Data2;
	k__u16 Data3;
	k__u8 Data4[8];
} efi_guid;

#define efi_graphic_output_protocol_guid                                       \
	{                                                                      \
		0x9042a9de, 0x23dc, 0x4a38,                                    \
		{                                                              \
			0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a         \
		}                                                              \
	}

#define efi_acpi_20_table_guid                                                 \
	{                                                                      \
		0x8868e871, 0xe4f1, 0x11d3,                                    \
		{                                                              \
			0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81         \
		}                                                              \
	}

typedef struct efi_graphics_output_protocol efi_graphics_output_protocol;

typedef enum {
	PixelRedGreenBlueReserved8BitPerColor,
	PixelBlueGreenRedReserved8BitPerColor,
	PixelBitMask,
	PixelBltOnly,
	PixelFormatMax
} efi_graphics_pixel_format;

typedef struct {
	k__u32 RedMask;
	k__u32 GreenMask;
	k__u32 BlueMask;
	k__u32 ReservedMask;
} efi_pixel_bitmask;

typedef struct {
	k__u32 Version;
	k__u32 HorizontalResolution;
	k__u32 VerticalResolution;
	efi_graphics_pixel_format PixelFormat;
	efi_pixel_bitmask PixelInformation;
	k__u32 PixelsPerScanLine;
} efi_graphics_output_mode_information;

typedef struct {
	k__u32 MaxMode;
	k__u32 Mode;
	efi_graphics_output_mode_information *Info;
	k__int SizeOfInfo;
	efi_physical_address FrameBufferBase;
	k__int FrameBufferSize;
} efi_graphics_output_protocol_mode;

typedef efi_status (*efi_graphics_output_protocol_query_mode)(
    efi_graphics_output_protocol *This, k__u32 ModeNumber, k__int *SizeOfInfo,
    efi_graphics_output_mode_information **Info);

typedef efi_status (*efi_graphics_output_protocol_set_mode)(
    efi_graphics_output_protocol *This, k__u32 ModeNumber);

typedef struct {
	k__u8 Blue;
	k__u8 Green;
	k__u8 Red;
	k__u8 Reserved;
} efi_graphics_output_blt_pixel;

typedef enum {
	EfiBltVideoFill,
	EfiBltVideoToBltBuffer,
	EfiBltBufferToVideo,
	EfiBltVideoToVideo,
	EfiGraphicsOutputBltOperationMax
} efi_graphics_output_blt_operation;

typedef efi_status (*efi_graphics_output_protocol_blt)(
    efi_graphics_output_protocol *This,
    efi_graphics_output_blt_pixel *BltBuffer,
    efi_graphics_output_blt_operation BltOperation, k__int SourceX, k__int SourceY,
    k__int DestinationX, k__int DestinationY, k__int Width, k__int Height, k__int Delta);

typedef struct efi_graphics_output_protocol {
	efi_graphics_output_protocol_query_mode QueryMode;
	efi_graphics_output_protocol_set_mode SetMode;
	efi_graphics_output_protocol_blt Blt;
	efi_graphics_output_protocol_mode *Mode;
} efi_graphics_output_protocol;

#define efi_system_table_signature 0x5453595320494249

typedef struct {
	k__u64 Signature;
	k__u32 Revision;
	k__u32 HeaderSize;
	k__u32 CRC32;
	k__u32 Reserved;
} efi_table_header;

typedef efi_status (*efi_locate_protocol)(efi_guid *Protocol,
					  void *Registration, void **Interface);

typedef efi_status (*efi_exit_boot_services)(efi_handle ImageHandle,
					     k__int MapKey);

#define efi_raise_tpl void *
#define efi_restore_tpl void *
#define efi_allocate_pages void *
#define efi_free_pages void *

#define UEFI_MMAP_SIZE 0x4000
#define efi_memory_descriptor k__u8
typedef efi_status (*efi_get_memory_map)(k__int *MemoryMapSize,
					 efi_memory_descriptor *MemoryMap,
					 k__int *MapKey, k__int *DescriptorSize,
					 k__u32 *DescriptorVersion);

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

typedef struct {
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
	efi_install_multiple_protocol_interfaces
	    InstallMultipleProtocolInterfaces;
	efi_uninstall_multiple_protocol_interfaces
	    UninstallMultipleProtocolInterfaces;

	efi_calculate_crc32 CalculateCrc32;

	efi_copy_mem CopyMem;
	efi_set_mem SetMem;
	efi_create_event_ex CreateEventEx;
} efi_boot_services;

typedef struct _efi_simple_text_output_protocol efi_simple_text_output_protocol;

typedef efi_status (*efi_text_string)(efi_simple_text_output_protocol *This,
				      utf16 *String);

typedef efi_status (*efi_text_reset)(efi_simple_text_output_protocol *This,
				     k__int flag);

#define efi_text_test_string void *
#define efi_text_query_mode void *
#define efi_text_set_mode void *
#define efi_text_set_attribute void *
#define efi_text_clear_screen void *
#define efi_text_set_cursor_position void *
#define efi_text_enable_cursor void *
#define simple_text_output_mode void *

typedef struct _efi_simple_text_output_protocol {
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
typedef struct {
	efi_guid VendorGuid;
	void *VendorTable;
} efi_configuration_table;

extern efi_graphics_output_protocol *gfx;

typedef struct {
	efi_table_header Hdr;
	utf16 *FirmwareVendor;
	k__u32 FirmwareRevision;
	efi_handle ConsoleInHandle;
	efi_simple_text_input_protocol *ConIn;
	efi_handle ConsoleOutHandle;
	efi_simple_text_output_protocol *ConOut;
	efi_handle StandardErrorHandle;
	efi_simple_text_output_protocol *StdErr;
	efi_runtime_services *RuntimeServices;
	efi_boot_services *BootServices;
	k__int NumberOfTableEntries;
	efi_configuration_table *ConfigurationTable;
} efi_system_table;

#define efi_usb2_hc_protocol_guid \
  {0x3e745226,0x9818,0x45b6,\
    {0xa2,0xac,0xd7,0xcd,0x0e,0x8b,0xa2,0xbc}}

typedef struct _efi_usb2_hc_protocol efi_usb2_hc_protocol;

typedef efi_status (*efi_usb2_hc_protocol_get_capability) (
	efi_usb2_hc_protocol       *This,
	k__u8                      *MaxSpeed,
  	k__u8                      *PortNumber,
  	k__u8                      *Is64BitCapable
);

typedef efi_status (*efi_async_usb_transfer_callback) (
	void                       *Data,
	k__uint                    DataLength,
	void                       *Context,
	k__u32                      Status
);

typedef struct {
	k__u8          TranslatorHubAddress;
	k__u8          TranslatorPortNumber;
} efi_usb2_hc_transaction_translator;

typedef efi_status (*efi_usb2_hc_protocol_async_isochronous_transfer) (
	efi_usb2_hc_protocol       *This,
	k__u8                      DeviceAddress,
	k__u8                      EndPointAddress,
	k__u8                      DeviceSpeed,
	k__uint                    MaximumPacketLength,
	k__u8                      DataBufferNumber,
	void			   *Data[7],
	k__uint                    DataLength,
	efi_usb2_hc_transaction_translator *Translator,
	efi_async_usb_transfer_callback IsochronousCallBack,
	void 			   *Context
);

typedef struct _efi_usb2_hc_protocol {
  efi_usb2_hc_protocol_get_capability		GetCapability;
/*
  EFI_USB2_HC_PROTOCOL_RESET                       Reset;
  EFI_USB2_HC_PROTOCOL_GET_STATE                   GetState;
  EFI_USB2_HC_PROTOCOL_SET_STATE                   SetState;
  EFI_USB2_HC_PROTOCOL_CONTROL_TRANSFER            ControlTransfer;
  EFI_USB2_HC_PROTOCOL_BULK_TRANSFER               BulkTransfer;
  EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER    AsyncInterruptTransfer;
  EFI_USB2_HC_PROTOCOL_ASYNC_INTERRUPT_TRANSFER    SyncInterruptTransfer;
  EFI_USB2_HC_PROTOCOL_ISOCHRONOUS_TRANSFER        IsochronousTransfer;
*/
  efi_usb2_hc_protocol_async_isochronous_transfer AsyncIsochronousTransfer;
 /* EFI_USB2_HC_PROTOCOL_GET_ROOTHUB_PORT_STATUS     GetRootHubPortStatus;
  EFI_USB2_HC_PROTOCOL_SET_ROOTHUB_PORT_FEATURE    SetRootHubPortFeature;
  EFI_USB2_HC_PROTOCOL_CLEAR_ROOTHUB_PORT_FEATURE  ClearRootHubPortFeature
*/
  k__u16                                           MajorRevision;
  k__u16                                           MinorRevision;
} efi_usb2_hc_protocol;


#endif /*__EFI_H__*/
