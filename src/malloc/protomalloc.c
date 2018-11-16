#define MALLOC
#define NULL 0

#ifndef TERMINAL
#include "terminal.c"
#endif

#define STRBUFF 32
#define STRSIZE 160
#define VARBUFF 256
#define VARNAMESIZE 16
#define VARVALUE 80
#define MALLOC_MAGIC 0x12345678
#define RECORDBOUNDS 400

#define PAGE_UNINITIALIZED 0
#define PAGE_INITIALIZED 1
#define PAGE_SIZE 4096
#define MALLOC_STRUCT_SIZE PAGE_SIZE

#include "multiboot.h"

//reocrds has:
// beginning
// width
// allocation status

typedef struct record {
	void* ptr; //4
	unsigned int size; // 4
	short is_taken; // 2
} record;

typedef struct malloc_struct{
	unsigned short first_free; // 2
	unsigned short unimplemented; // 2
	unsigned int total_free_memory; // 4
	record data[RECORDBOUNDS]; // 10 * 400
} malloc_struct;

// this is a reference to an object container with page count
typedef struct allocation {
	malloc_struct* malloc_struct_addr; //4
	unsigned int len_pages; //4
	unsigned int status; // ==0 uninitialized; 1 == used; 2 == system; 3 = uninitialized etc...
} allocation;

// this is a list of containers of objects
typedef struct malloc_table {
	unsigned int magic; //4
	unsigned int first_page; //4
	unsigned int num_pages; //4
	struct malloc_table* next_table; //8
	//12 bytes free
	allocation malloc_structs[254];
} malloc_table;

multiboot_info_t* malloc_table_pointer = NULL;
multiboot_memory_map_t* primary_malloc_table = NULL;
const void* malloc_candidates[] = {(void*) 0x1000000, NULL};
malloc_table* malloc_master_table = NULL;

int mmap_is_available(const void* address, int length){
	//TODO
	(void) address;
	(void) length;
	return 1;
	//test if in available in multibootmemmap
	//test if all zeros
}

void make_malloc(multiboot_info_t* mbd){
	if ((mbd->flags & 0x00000001) == 0){
		die("Memory amounts not ready");
	}
	if ((mbd->flags & 0x00000020) == 0){
		die("Memory map unavailable");
	}
	malloc_table_pointer = mbd;
	multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)(long int)(mbd->mmap_addr);
	while(mmap < (multiboot_memory_map_t*) (unsigned long int) (mbd->mmap_addr + mbd->mmap_length)) {
		/*
		int higher = (unsigned int) (mmap->addr >> 32);
		int lower = (unsigned int) (mmap->addr);
		int higher_len = (unsigned int) (mmap->len >> 32);
		int lower_len = (unsigned int) (mmap->len);
		int type = mmap->type;
		*/
		// NEED TO: skip STACK area and CODE area and detect real unused memory, not only non-mapped non-damaged memory
		mmap = (multiboot_memory_map_t*) ( (unsigned long int) mmap + mmap->size + sizeof(mmap->size) );
	}
	int index = 0;
	while(malloc_candidates[index]){
		if(mmap_is_available(malloc_candidates[index], PAGE_SIZE)){
			malloc_master_table = (malloc_table*) malloc_candidates[index];
			break;
		} else {
			index++;
		}
	}
	if(!malloc_master_table){
		die("Cannot create malloc master table");
	} else {
		(*malloc_master_table) = (malloc_table) {MALLOC_MAGIC, (unsigned long int) malloc_master_table, 4, NULL, {{0}}};
		unsigned long int next_page = (unsigned long int) malloc_master_table + PAGE_SIZE;
		malloc_master_table->malloc_structs[0] = (allocation) {(malloc_struct*) next_page, 3, PAGE_INITIALIZED};
		(* (malloc_struct*) next_page) = (malloc_struct) {0, 0, 2 * PAGE_SIZE, {{0}}};
	}
}

void log_memory(){
	multiboot_info_t* mbd = malloc_table_pointer;
	multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)(long int)(mbd->mmap_addr);
	int size = (unsigned int) (mmap->size) + sizeof(mmap->size);
	terminal_cat("Available records: ");
	terminal_cat(itoa(mbd->mmap_length / size));
	terminal_cat("\n");
	while(mmap < (multiboot_memory_map_t*) (unsigned long int) (mbd->mmap_addr + mbd->mmap_length)) {
		int higher = (unsigned int) (mmap->addr >> 32);
		int lower = (unsigned int) (mmap->addr);
		int higher_len = (unsigned int) (mmap->len >> 32);
		int lower_len = (unsigned int) (mmap->len);
		int type = mmap->type;
		terminal_cat("HI: ");
		terminal_cat(itoa(higher));
		terminal_cat(" LO: ");
		terminal_cat(itoa(lower));
		terminal_cat(" LEN_HI: ");
		terminal_cat(itoa(higher_len));
		terminal_cat(" LEN_LO: ");
		terminal_cat(itoa(lower_len));
		if(type == 1){
			terminal_cat(" AVAILABLE");
		} else if(type == 2){
			terminal_cat(" RESERVED");
		} else if(type == 3){
			terminal_cat(" RECLAIMABLE");
		} else if(type == 4){
			terminal_cat(" NVS");
		} else if(type == 5){
			terminal_cat(" BROKEN");
		} else {
			terminal_cat(" TYPE: ");
			terminal_cat(itoa(type));
		}			
		terminal_cat("\n");
		mmap = (multiboot_memory_map_t*) ( (unsigned long int) mmap + mmap->size + sizeof(mmap->size) );
	}

}

void* malloc(unsigned int size){
	allocation candidate = malloc_master_table->malloc_structs[0];
	void* ptr = NULL;
	while(!ptr){
		if(candidate.status == PAGE_UNINITIALIZED){
			//change in the future
			die("Allocing to uninitialized struct");
		}
		char valid = 0;
		unsigned int first_free = candidate.malloc_struct_addr->first_free;
		// TODO check if first_free is non -1
		while(!valid){
			if(candidate.malloc_struct_addr->total_free_memory > size){
				if(candidate.malloc_struct_addr->first_free != 0){
					record rec = candidate.malloc_struct_addr->data[first_free - 1];
					ptr = (void*) ((unsigned long int) rec.ptr + rec.size);
				} else {
					ptr = candidate.malloc_struct_addr + PAGE_SIZE;
				}
				// validate if doesnt overlap with next ptr
				if(first_free < RECORDBOUNDS){
					unsigned int next_taken = 0;
					for(int i = first_free + 1; first_free < RECORDBOUNDS; i++){
						if(candidate.malloc_struct_addr->data[i].is_taken){
							next_taken = i;
							break;
						}
					}
					if(!next_taken){
						valid = 1;
						terminal_cat("This is generally valid\n");
					} else {
						if((candidate.malloc_struct_addr->data[next_taken].ptr
							- candidate.malloc_struct_addr->data[first_free].ptr) < (signed long int) candidate.malloc_struct_addr->data[first_free].size){
							valid = 1;
						} else {
							// find a new first_free
							unsigned int found_new_first_free = 0;
							for(int i = first_free + 1; first_free < RECORDBOUNDS; i++){
								if(!candidate.malloc_struct_addr->data[i].is_taken){
									found_new_first_free = i;
									break;
								}
							}
							if(found_new_first_free){
								terminal_cat("New first free candidate: ");
								terminal_cat(itoa(found_new_first_free));
								terminal_cat("\n");
								first_free = found_new_first_free;
								continue;
							} else {
								die("Need another alloc struct - not implemented");
								// TODO need to take another malloc struct
							}
						}						
					}
				} else {
					die("Candidate index out of bounds");
				}
				if(valid){
					terminal_cat("Found valid\n");
					candidate.malloc_struct_addr->data[first_free].ptr = ptr;
					candidate.malloc_struct_addr->data[first_free].size = size;
					candidate.malloc_struct_addr->data[first_free].is_taken = 1;
					// REMOVE SIZE FROM TOTAL SIZE - TODO
					char done = 0;
					// ALIGN FIRST FREE
					for(unsigned int i = first_free + 1; i < MALLOC_STRUCT_SIZE; i++){
						if(!candidate.malloc_struct_addr->data[i].is_taken){
							candidate.malloc_struct_addr->first_free = i;
							done = 1;
							break;
						}
					}
					if(!done){
						candidate.malloc_struct_addr->first_free = -1;
						die("Not implemented: allocating a new struct");
						// make a new struct and forget about this one
					}
					break; // exit loop to return new ptr
				}
			} else {
				die("Not implemented: allocating new malloc struct");
				// if there's not enough size, pick new malloc table if it exists
			}
		}
	}
	terminal_cat("Malloc'd successfully\n");
	return ptr;
}

void free(void* ptr){
	(void) ptr;
}

void mmap_clean(){
	
}

void print_malloc_map(malloc_struct* s){
	terminal_cat("Malloc map: ");
	terminal_cat("\n");
	for(int i=0; i < RECORDBOUNDS; i++){
		if(s->data[i].is_taken){
			terminal_cat("ADDR: ");
			terminal_cat(itoa((unsigned int) (unsigned long int) s->data[i].ptr));
			terminal_cat(" LEN: ");
			terminal_cat(itoa(s->data[i].size));
			terminal_cat("\n");
		}
	}
}

void malloc_info(){
	// for debugging purposes no atoi...
	print_malloc_map(malloc_master_table->malloc_structs[0].malloc_struct_addr);
}