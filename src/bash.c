#define BASH

#ifndef TERMINAL
#include "terminal.c"
#endif
#ifndef MALLOC
#include "malloc.c"
#endif
#ifndef UTILS
#include "utils.c"
#endif

#define NUMCMDS 10

int bash_cat(char *s){
    terminal_cat(s);
    terminal_cat("\n");
    return 0;
}

int bash_meminfo(char *s){
    (void) s;
    terminal_cat("System string blocks: ");
    terminal_cat(itoa(malloc_memuse()));
    terminal_cat("\n");
    terminal_cat("Terminal variables: ");
    terminal_cat(itoa(malloc_varuse()));
    terminal_cat("\n");
    return 0;
}

int bash_cls(char *s){
    (void) s;
    terminal_cls();
    return 0;
}

int bash_set(char *s){
    char buff[16] = {0};
    int sep = strFindCh(s, ' ');
    if(sep!=-1) {
        if(sep > 15){
            terminal_cat("Too long varname\n");
            return 1;
        }
        if(s[sep+1]){
            for(short i=0;i<sep;i++){
                buff[i] = s[i];
            }
            buff[sep] = 0;
            setVariable(buff, &s[sep+1]);
            return 0;
        }
    }
    terminal_cat("Setvar syntax error\n");
    return 1;
}

int bash_printvar(char *s){
    terminal_cat(getVariable(s));
    terminal_cat("\n");
    return 0;
}

int bash_color(char *s){
    if(s[0]>='0' && s[0]<='7' && s[1]>='0' && s[1]<='7')
    {
        terminal_setcolor(vga_color(s[0] - '0', s[1] - '0'));
    }
    return 0;
}

int bash_memmap(char* s){
    (void) s;
	log_memory();
	return 0;
}

int bash_get_malloc_page(char* s){
	(void) s;
	// for debugging purposes no atoi...
	malloc_info();
	return 0;
}

typedef struct shcom {
    char name[15];
    int (*function)(char *s);
} shcom;

shcom COMMANDS[NUMCMDS];

void setupCommands(){
    setStr(COMMANDS[0].name, "cat ");
    COMMANDS[0].function = &bash_cat;
    setStr(COMMANDS[1].name, "meminfo");
    COMMANDS[1].function = &bash_meminfo;
    setStr(COMMANDS[2].name, "set ");
    COMMANDS[2].function = &bash_set;
    setStr(COMMANDS[3].name, "printvar ");
    COMMANDS[3].function = &bash_printvar;
    setStr(COMMANDS[4].name, "color ");
    COMMANDS[4].function = &bash_color;
    setStr(COMMANDS[5].name, "cls");
    COMMANDS[5].function = &bash_cls;
    setStr(COMMANDS[6].name, "memmap");
    COMMANDS[6].function = &bash_memmap;
    setStr(COMMANDS[7].name, "malloc_info");
    COMMANDS[7].function = &bash_get_malloc_page;
}


void bashRun(char *s){
    for(short i=0; i<NUMCMDS; i++){
        if(COMMANDS[i].name[0]) {
            if(cmpStrHead(s, COMMANDS[i].name)){
                char *arg = &s[strLen(COMMANDS[i].name)];
                if(COMMANDS[i].function(arg))
                    terminal_cat("Function exited with error!\n");
                return;
            }
        }
    }
    terminal_cat("No such command!\n");
}

void bash(){
  setupCommands();
  char *s = newString();
  while(1){
      setStr(s, terminal_inputLine());
      bashRun(s);
  }
}
