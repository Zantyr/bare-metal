// filesystem of shitOS
// ls - lists all files with names beginning with a string
// read - opens a file and copies its contents to a new bytestream
// create - change a handle

// requires list library
// which requires better macro capabilities

#ifndef STDLIB
#include "stdlib.c"
#endif

typedef struct File{
	int length;
	char * name;
	char * contents;
} File;

typedef File* FileRef;

defineList(FileRef)

List(FileRef) filesystem = NULL;

List(FileRef) listdir(char * initial){
	List(FileRef) new_list = malloc(sizeof(List(FileRef)));
	for(int index=0;index < listLen(FileRef)(filesystem);index++){
		if(initial){
			if(cmpStrHead(get(FileRef)(filesystem, index)->name, initial)){
				append(FileRef)(new_list, get(FileRef)(filesystem, index));
			}
		} else {
			append(FileRef)(new_list, get(FileRef)(filesystem, index));
		}
	}
	return new_list;
}
