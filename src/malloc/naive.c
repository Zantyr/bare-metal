#define MALLOC
#define MALLOCNAME "naive"

void * master_pointer = (void *) 0x1000000;

int make_malloc(multiboot_info_t * magic){
	(void) magic;
	return 1;
}

void * malloc(int bytes){
	void * ptr = master_pointer;
	master_pointer += bytes;
	return ptr;
}

void log_memory(){
	terminal_cat("Current malloc pointer: ");
	terminal_cat(itoa((int) master_pointer));
	terminal_cat("\n");
}

void malloc_info(){
	terminal_cat("No memory management enabled\n");
}