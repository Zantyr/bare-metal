#define UTILS

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void outb(unsigned short port, char data) {
    asm volatile (
        "outb %0, %1"
        : : "a"(data)
        , "Nd"(port)
    );
}

void halt(){
    while(1);
}

void setStr(char *to, char *from){
    short index = 0;
    while(from[index] != 0){
        to[index] = from[index];
        index++;
    }
    to[index] = 0;
}

int cmpStrHead(char *from, char *to) {
    short index = 0;
    while(to[index] != 0){
        if(from[index] != to[index])
            return 0;
        index++;
    }
    return 1;
}

short strLen(char *s){
    short index=0;
    while(s[index]){
        index++;
    }
    return index;
}

void strRev(char *s){
    short sl = strLen(s) - 1;
    char buff = 0;
    for(short ix = 0; ix < (sl + 1)/2;ix++) {
        buff = s[ix];
        s[ix] = s[sl - ix];
        s[sl - ix] = buff;
    }
}

int strFindCh(char *s, char wha){
    int index = 0;
    while(s[index]) {
        if(s[index] == wha)
            return index;
        index++;
    }
    return -1;
}

char itoaBuffer[] = "             ";

//fix reverse pushing xDDD
char * itoa(unsigned int i) {
    int digit;
    short index = 0;
    if(i == 0){
    	itoaBuffer[index] = '0';
    	index++;
    }
    else {
	    while(i != 0){
   	        digit = i % 10;
   	        i /= 10;
   	        itoaBuffer[index] = '0' + (char) digit;
   	        index++;
   	    }   	    	
    }
    itoaBuffer[index] = 0;
    strRev(itoaBuffer);
    return itoaBuffer;
}