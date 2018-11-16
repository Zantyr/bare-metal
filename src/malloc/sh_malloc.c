#define MALLOC
#define NULL 0

#ifndef TERMINAL
#include "../terminal.c"
#endif

#define STRBUFF 32
#define STRSIZE 160
#define VARBUFF 256
#define VARNAMESIZE 16
#define VARVALUE 80
#define MALLOC_MAGIC 0x12345678
#define RECORDBOUNDS 400

#include "../multiboot.h"

//// LEGACY MALLOC FOR "BASH"
//primitive Strings


char stringMap[STRBUFF*STRSIZE] = {0};
signed short strCount = 0;

char varMap[VARBUFF*(VARNAMESIZE+VARVALUE)] = {0};

char * newString() {
    for(short i = 0;i<STRBUFF;i++){
        if(stringMap[i*STRSIZE] == 0) {
            strCount ++;
            return &stringMap[STRBUFF*STRSIZE];
        }
    }
    die("No memory for a new string ;(");
    return stringMap;
}

void destroyString(char *s){
    int offset = ((unsigned int) s) - ((unsigned int) stringMap);
    if(offset > STRBUFF*STRSIZE){
        die("Trying to destroy nonexistent string");
    }
    int index = (offset / STRSIZE);
    for(short i=0; i<STRSIZE; i++){
        stringMap[index * STRSIZE + i] = 0;
        strCount--;
    }
}

char * getVariable(char *name){
    for(short i=0; i<STRBUFF;i++) {
        if(varMap[i*(VARNAMESIZE+VARVALUE)]!=0){
            if(cmpStrHead(name, &varMap[i*(VARNAMESIZE+VARVALUE)])) {
                return &varMap[i*(VARNAMESIZE+VARVALUE) + VARNAMESIZE];
            }
        }
    }
    terminal_cat("Warning: unset variable\n");
    return "";
}

void setVariable(char *name, char *val){
    for(short i=0; i<STRBUFF; i++) {
        if(varMap[i*(VARNAMESIZE+VARVALUE)]==0){
            setStr(&varMap[i*(VARNAMESIZE+VARVALUE)], name);
            setStr(&varMap[i*(VARNAMESIZE+VARVALUE)+VARNAMESIZE], val);
            return;
        }
    }
    die("No memory for a new variable");
}

int malloc_memuse(){
    return (int) (STRBUFF - strCount);
}

unsigned int malloc_varuse(){
    return (unsigned int) VARBUFF;
}
