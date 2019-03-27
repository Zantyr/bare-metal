#ifndef TERMINAL
#include "terminal.c"
#endif
#ifndef UTILS
#include "utils.c"
#endif
#ifndef MALLOC
#include "malloc/sh_malloc.c"
#ifdef DESIREDMALLOCISPROTO
#include "malloc/protomalloc.c"
#endif
#if DESIREDMALLOCISNAIVE
#include "malloc/naive.c"
#endif
#endif
#ifndef BASH
#include "bash.c"
#endif
#ifndef MULTIBOOT
#include "multiboot.h"
#endif

#include "init.c"

void kernel_main(multiboot_info_t* mbd){
        terminal_initialize();
		// make a malloc
		make_malloc(mbd);
		// loaded correctly, you may proceed
		init_filesys();
        terminal_cat("This is shitOS!\n");
        terminal_cat("Version 0.0.1\n");
        terminal_cat("Used malloc: ");terminal_cat(MALLOCNAME);terminal_cat("\n");
		if(init()){
			terminal_cat("Init returned zero - fallback to console\n");
			bash();
		}
}
