






https://wiki.osdev.org/Detecting_Memory_(x86)

Section memory map via GRUB

include multiboot.h

After loading from GRUB
	store EBX - 
	push EBX onto stack - this is a pointer to your memory map

	_main should accept _main(multiboot_info_t* mbd, unsigned int magic)

	magic is to be stored and verified; multiboot_info is a struct with various multiboot data

	mbd->flags - assert bit 0 is set
	mbd->mem_upper - conventional memory map

	assert mbd->flags 6 bit is set
	mbd->mmap_addr to get the memory map

	mbd->mmap_addr contains mmbd->mmap_length of records of type:
		{
			-4 size
			0 base_addr_low
			4 base_addr_hi
			8 len_low
			12 len_hi
			16 type
		}

	if type == 1 - page is available

	get memory map, allocate OS memmap structure and then you may safely malloc to subprocesses ;)