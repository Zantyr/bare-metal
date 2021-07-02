// Kernel definitions

#define U64 long long unsigned int
void print_si(char* s, int i); // prints "{string}: {int}\n"
void panic(char* s);

// Page table allocator
#define SEGMENT_TABLE_SIZE 16

typedef struct Segment {
	void* start;
	void* bitmap_start;
	void* data_start; // this is offset after the bit_table
	int length; // in 4k pages
	int last_allocation; // starting point for search, a la circular buffer
} Segment;

static Segment masterSegmentTable[SEGMENT_TABLE_SIZES] = {{0, 0, 0, 0, 0}};

Segment* make_segment(void* from, void* to){
	U64 length = (U64) to - (U64) from;
	length /= 4096;
	int segments_to_allocate = 1 + (1 + length + sizeof(Segment)) / 4096;
	length -= (1 + length + sizeof(Segment)) / 4096;
	Segment segment = {
		start: from,
		data_start: from + (4096 * segments_to_allocate),
		length: length
	};
	*(Segment *)from = segment;
	// TODO: Initialize segment map
	return from;
}

void *alloc_4k_page() {
	for(int i = 0; i < SEGMENT_TABLE_SIZE; i++) {
		if (masterSegmentTable[i].length == 0) {
			continue;
		}
		unsigned int *bitmapStart = masterSegmentTable[i].bitmap_start;
		unsigned int *ptr = masterSegmentTable[i].bitmap_start + masterSegmentTable[i].last_allocation;
		while(ptr - bitmapStart < masterSegmentTable[i].length / 8){
			if(*ptr == 0xFFFFFFFF) {
				ptr += sizeof(int);
			} else {
				int offset = firstEmptyBit(*ptr);
				*ptr = *ptr & (2 << offset);
				masterSegmentTable[i].last_allocation = ptr - bitmapStart;
				return masterSegmentTable[i].data_start + 4096 * (8 * (ptr - bitmapStart) + offset);
			}	
		}
		*ptr = masterSegmentTable[i].bitmap_start;
		while(ptr - bitmapStart < masterSegmentTable[i].last_allocation){
			if(*ptr == 0xFFFFFFFF) {
				ptr += sizeof(int);
			} else {
				int offset = firstEmptyBit(*ptr);
				*ptr = *ptr & (2 << offset);
				masterSegmentTable[i].last_allocation = ptr - bitmapStart;
				return masterSegmentTable[i].data_start + 4096 * (8 * (ptr - bitmapStart) + offset);
			}	
		}
	}
	panic("Cannot allocate 4k page!");
}

void *alloc_128k_arena() {
	for(int i = 0; i < SEGMENT_TABLE_SIZE; i++) {
		if (masterSegmentTable[i].length == 0) {
			continue;
		}
		unsigned int *ptr = masterSegmentTable[i].bitmap_start + masterSegmentTable[i].last_allocation;
		unsigned int *bitmapStart = masterSegmentTable[i].bitmap_start;
		while(ptr - bitmapStart < masterSegmentTable[i].length / 8){
			if(*ptr != 0x00000000) {
				ptr += sizeof(int);
			} else {
				*ptr = 0xFFFFFFFF;
				masterSegmentTable[i].last_allocation = ptr - bitmapStart;
				return masterSegmentTable[i].data_start + 4096 * (8 * (ptr - bitmapStart));
			}
		}
		*ptr = masterSegmentTable[i].bitmap_start;
		while(ptr - bitmapStart < masterSegmentTable[i].last_allocation){
			if(*ptr != 0x00000000) {
				ptr += sizeof(int);
			} else {
				*ptr = 0xFFFFFFFF;
				masterSegmentTable[i].last_allocation = ptr - bitmapStart;
				return masterSegmentTable[i].data_start + 4096 * (8 * (ptr - bitmapStart));
			}	
		}
	}
	panic("Cannot allocate 128k arena!");
}

void free_4k_page() {
	
}

typedef struct Arena {
	unsigned short size;
	void* bitfieldStart;
	void* dataStart;
} Arena;

Arena *make_arena(unsigned short size) {
	
}

typedef struct MasterAllocator {
	Arena arena[16];
	ListOfPages* list_of_pages;
} MasterAllocator;

static MasterAllocator MASTER_ALLOCATOR = {0};

void* malloc(size_t size){
	if(size_t < 65535){
		return _malloc_on_arena()
	}
	return _malloc_on_pages()
}

/*
For objects in size below 64k-1 in size, create an arena
For objects 64k+ - reserve contiguous pages by hand and store allocations in external linked list

Arenas are managed in two mappings (b-trees):
- from address prefix to type of allocation (arena // pages)
- from size to arena list

Allocating on arena:
- find a free space in ready arenas and allocate there
- if all arenas are busy, create a new arena
- if cannot create an arena, find whether there are free arenas in any size and free some
- last freed arena may be reallocated to the current size
- if no arena was freed, OOM error
- zero the allocated space

Allocating on heap:
- request contiguous pages
- if not possible, OOM
- add allocation to the allocation list
- zero the allocated space

Allocating on stack (alloca):
- ??? some assembly shiet

Freeing:
- determine from the type of pointer, in which storage it is stored
- if in arena, find arena bitfield and size and zero a proper bitfield
- if on heap, find a proper tuple starting from the address, remove it and free appropriate pages

*/

// TODO: Memory viewer
// gives you info on what is on a given memory location
// uses colored display
