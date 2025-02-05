//                        OS-3o3 Operating System
//
//                      08 October MMXXIV PUBLIC DOMAIN
//           The author disclaims copyright to this source code.
//
// https://github.com/eszkadev/UEFI-32bit-asm-examples/tree/master
 
#include "../../include/arch/i386/defs.h"

.code32
.text

.global _k__printf
_k__printf:
    jmp _k__printf2

.globl _k__memcpy
_k__memcpy:
    movl	4(%esp),%edi	# dest
    movl	8(%esp),%esi    # src
    movl	12(%esp),%ecx   # n
	rep movsb
	ret

.global _load_gdt
_load_gdt:
    
	lgdtl gdtp
	lidtl idt 

    movl $0x10, %eax
	movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

	movl $MEM_KERNEL_STACK, %esp # set the kernel stack just below the load address
	movl $MEM_KERNEL_STACK, %ebp

    ljmpl $0x8, $KERNEL_ENTRY32

.align 16
gdtp:
    .word gdt_end - gdt - 1
    .long gdt

.align 16
gdt:
gdt_null:
    .quad 0
gdt_code_segment:
    .word 0xffff
    .word 0x0000
    .byte 0x00
    .byte 0b10011010
    .byte 0b11001111
    .byte 0x00
gdt_data_segment:
    .word 0xffff
    .word 0x0000
    .byte 0x00
    .byte 0b10010010
    .byte 0b11001111
    .byte 0x00
gdt_end:

idt:
    .word 0
    .long 0


// https://github.com/llvm/llvm-project/issues/17631
.global @feat.00
.set @feat.00, 1
 
	.p2align 12,0
endcodes:

#.set codesize, endcodes - _efi_main 


datastart:

.long 4

dataend:
	.rept (4096 - (dataend - datastart) % 4096)
	.byte 0
	.endr
dataends:

.set datasize, dataends - datastart
