#ifndef FILESYSTEM
#include "fs.c"
#endif
#ifndef TERMINAL
#include "terminal.c"
#endif

#define HEX

#define ESC_SCAN 1

char hexDigit(char decimal){
  if(decimal >= 0 && decimal < 10){
    return '0' + decimal;
  } else if(decimal >= 10 && decimal < 16){
    return 'A' + decimal - 10;
  } else {
    return 0;
  }
}

void intToHex(int val, char* str){
  str[0] = hexDigit(0xF & (val >> 28));
  str[1] = hexDigit(0xF & (val >> 24));
  str[2] = hexDigit(0xF & (val >> 20));
  str[3] = hexDigit(0xF & (val >> 16));
  str[4] = hexDigit(0xF & (val >> 12));
  str[5] = hexDigit(0xF & (val >> 8));
  str[6] = hexDigit(0xF & (val >> 4));
  str[7] = hexDigit(0xF & (val >> 0));
}

int hexToInt(char* str){
  int val = 0;
  for(int i=0; i<8; i++){
    val = val << 4;
    if(str[i] >= '0' && str[i] <= '9'){
      val += str[i] - '0';
    } else if(str[i] >= 'A' && str[i] <= 'F') {
      val += str[i] - 'A' + 10;
    } else {
      break;
    }
  }
  return val;
}

int bash_peek(char *str){
  // if wrong addr, return 1 prematurely
  int addr = hexToInt(str);
  int addrVal = *((int*) addr);
  char* addrValStr = (char*) malloc(9);
  intToHex(addrVal, addrValStr);
  terminal_cat("Memory at ");
  terminal_cat(str);
  terminal_cat(":");
  terminal_cat(addrValStr);
  terminal_cat("\n");
  return 0;
}

typedef struct HexEditView {
    FileWriter fileHandler;
    int cursorX;
    int cursorY;
    char buffState;
    int page;
    int length;
} *HexEditView;

void fileInsertChar(){

}

void redrawHexEditView(HexEditView view){
  terminal_cls();
  // for each 16 lines draw each 16 pairs and corresponding
  // safe string
  // add a header with "/ for help" and current file and location
  (void) view;
  for(int position_x = 0; position_x < 16; position_x ++){
	  for(int position_y = 0; position_y < 16; position_y ++){
	  	  int position = position_y * 16 + position_x;
		  char drawable = view->fileHandler->content[position];
		  terminal_write_symbol(hexDigit(drawable >> 4), 0xF, 3 * position_x, position_y);
		  terminal_write_symbol(hexDigit(drawable & 0xF), 0xF, 3 * position_x + 1, position_y);
      }
  }
};

int interpretHexEditViewCmd(HexEditView view, char cmd){
  (void) view;
  // controls: digits + abcdef - input
  // hljk - move
  // i - insert empty byte
  // r - remove byte
  // / - help
  // enter - exit to menu
  // ,. - switch pages
  // ; - set EOF position
  // n - fill page
  // w - insert word - 4 bytes
  // s - save
  if(cmd == ESC_SCAN){
    return -1;
  }
  return 0;
}

int hexEditMainLoop(FileWriter writer){
  struct HexEditView viewObj = {
    writer, 0, 0, 0, 0, 0
  };
  HexEditView view = &(viewObj);
  redrawHexEditView(view);
  while(1){
    char cmd = terminal_get_scan_code();
    int value = interpretHexEditViewCmd(view, cmd);
    if(value){
      return value;
    }
  }
}

int bash_hexedit(char *str){
  FileRef fileref = getFile(str);
  FileWriter writer = NULL;
  struct FileWriter writerAlloc = {NULL, 0, 0, NULL};
  if(fileref){
    writer = &(writerAlloc);
    writer->fileRef = fileref;
    fileWriterLoad(writer);
  }
  // identify file
  if(writer){
    int errCode = hexEditMainLoop(writer);
    terminal_cls(); // clean and return
    terminal_cat("Exited hexedit\n");
    if(errCode == -1){
      return 0;
    }
    return errCode;
  } else {
    terminal_cat("hexed: Null file - not found or error\n");
    return 1;
  }
}
