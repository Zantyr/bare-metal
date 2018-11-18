#include "fs.c"

int init(){
	(void) malloc(5);
	terminal_cat("Init started - looping forever\n");
	while(1){
	}
	return 1;
}