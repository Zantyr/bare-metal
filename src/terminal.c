/* This tutorial will only work for the 32-bit ix86 targets. */
#define TERMINAL

#ifndef UTILS
#include "utils.c"
#endif


enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};


static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

char UPPERCASE[] = {0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 127, '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D', 'F' ,'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char LOWERCASE[] = {0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 127, '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f' ,'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int terminal_row;
int terminal_column;
unsigned char terminal_color;
unsigned short* terminal_buffer;

static inline unsigned char vga_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline unsigned short vga_symbol(unsigned char uc, unsigned char color) {
	return (unsigned short) uc | (unsigned short) color << 8;
}

void terminal_cls() {
    terminal_row = 0;
    terminal_column = 0;
    for (int y = 0; y < VGA_HEIGHT; y++){
        for(int x = 0; x < VGA_WIDTH; x++){
            const int index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_symbol(' ', terminal_color);
        }
    }
}

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (unsigned short*) 0xB8000;
}

void terminal_setcolor(unsigned char color){
	terminal_color = color;
}

void terminal_write_symbol(char c, unsigned char color, int x, int y){
	const int pos = y * VGA_WIDTH + x;
	terminal_buffer[pos] = vga_symbol(c, color);
}

void terminal_put_char(char c){
	terminal_write_symbol(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == (VGA_HEIGHT-1)){
                    for(short row=0; row<(VGA_HEIGHT-2); row++){
                        for(short col=0;col<VGA_WIDTH;){
                            terminal_buffer[(row-1) * VGA_WIDTH + col] = terminal_buffer[row * VGA_WIDTH + col];
                        }
                    }
                    for(short col=0;col<VGA_WIDTH;){
                        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] = ' ';
                    }
                }
	}
}

void terminal_write_char(char c){
    if(c == '\r' || c == '\n'){
        terminal_column = 0;
        if(terminal_row < (VGA_HEIGHT-1)){
        	terminal_row++;
        } else {
        	for(int i=0; i < VGA_HEIGHT - 1; i++){
        		for(int j=0; j < VGA_WIDTH; j++){
					const int pos = i * VGA_WIDTH + j;
					const int pos2 = (i+1) * VGA_WIDTH + j;
					terminal_buffer[pos] = terminal_buffer[pos2];
        		}
        	}
   	  		for(int j=0; j < VGA_WIDTH; j++){
				terminal_write_symbol(' ', terminal_color, j, VGA_HEIGHT - 1);
       		}        	
        }
    } else if(c == 127) {
        if(terminal_column) terminal_column--;
        terminal_write_symbol(' ', terminal_color, terminal_column, terminal_row);

    } else if(c >= ' '){
        terminal_put_char(c);
    } else {
        terminal_put_char('.');
    } 
    unsigned short position=(terminal_row * VGA_WIDTH) + terminal_column;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

void terminal_cat(const char* string){
	unsigned char i = 0;
	while(string[i]){
            terminal_write_char(string[i]);
            i++;
	}
}

char lastScancode = 0;

static char terminal_get_scan_code() {
    char c=0;
    do {
        if(inb(0x60)!=c) {
            c=inb(0x60);
            if(c!=lastScancode){
                lastScancode = c;
                if(c>0) return c;
            }
        }
    } while(1);
}

static char terminal_getASCII(){
    while(1) {
        char chr = terminal_get_scan_code();
        if(chr == 42 || chr == 54){
            char asc = UPPERCASE[(unsigned char) terminal_get_scan_code()];
            if(asc) return asc;
        } else {
            char asc = LOWERCASE[(unsigned char) chr];
            if(asc) return asc;
        }
    }
}

char inputBuffer[80] = {0};

static char * terminal_inputLine(){
    for(short i=0; i<80; i++) inputBuffer[i]=0;
    short index = 0;
    char inputChar = 0;
    do {
        inputChar = terminal_getASCII();
        if(inputChar==27){
            for(short i=0; i<80; i++) inputBuffer[i]=0;
            index = 0;
        } else if(inputChar==127) {
            if(index){
                inputBuffer[index] = 0;
                index--;
                terminal_write_char(inputChar);
            }
        } else if(inputChar=='\n') {
            terminal_write_char(inputChar);
            break;
        } else if(index<80) {
            inputBuffer[index] = inputChar;
            index++;
            terminal_write_char(inputChar);
        }
    } while(inputChar);
    return inputBuffer;
}

void die(char *s){
    terminal_cat("KERNEL FAULT!\n");
    terminal_cat(s);
    halt();
}