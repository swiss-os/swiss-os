
/*
 *                       OS-3o3 Operating System
 *
 *                      12 November MMXXV PUBLIC DOMAIN
 *           The authors disclaim copyright to this source code.
 *
 *
 */

#include "efi.h"
#include "klib.h"

extern void *ImageHandle;
extern efi_system_table *SystemTable;
extern efi_simple_text_output_protocol *ConOut;

void efios__main(void)
{
	k__init_term();

	k__printf("\nHEllo\n");
	while (1) ;
}

