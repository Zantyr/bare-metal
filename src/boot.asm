; in the beginning there was a bootloader

MBALIGN    equ 1<<0        ; ???
MEMINFO    equ 1<<1        ; pls gibe me memmap
MAGIC      equ 0x1BADB002  ; this indicates that this is compatible with bootloader
FLAGS      equ MBALIGN | MEMINFO    ; build flags
global CHECKSUM
CHECKSUM   equ -(MAGIC + FLAGS)     ; checksum, just this

section .multiboot   ;our multiboot header
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss ; and God said, let there be a stack
stack_bottom:
resb 16384; 16KiB of memory
stack_top:
; and there was a stack

section .text
global _start:function (_start.end - _start)  ; O.O
_start: ; this label is recognized as the entry point of teh kernelz
	; now we're in 32-bit protected mode - no paging, no interrupts, 
	; and God divided stack from the rest of code
	mov esp, stack_top
	push ebx ; push data on the memory map; is it aligned?
	extern kernel_main
	call kernel_main
	cli
.hang:	hlt
	jmp .hang
.end:
