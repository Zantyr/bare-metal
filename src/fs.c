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

#ifndef LISTTYPEFileRef
typedef struct FileRefList__ {
	FileRef value;
	struct FileRefList__ *next;
} FileRefList__ ;

static FileRef get_FileRef(FileRefList__ *list, unsigned int index){
	for(;index>0;index--) list = list->next;
	return list->value;
}

static void push_FileRef(FileRefList__ *list, FileRef value) {
	while(list->next) list = list->next;
	list->next = malloc(sizeof(FileRefList__));
	list->next->value = value;
}
static FileRefList__ * remove_FileRef(FileRefList__ *list, FileRef value) {
	if(list->value == value){
		FileRefList__ * toret =  list->next;
		free(list);
		return toret;
	}
	FileRefList__ * toret = list;
	while(list->next){
		if(list->next->value == value){
			FileRefList__ * toappend = list->next->next;
			free(list->next);
			list->next = toappend;
			return toret;
		}
		list = list->next;
	}
	return NULL;
}
static int listLen_FileRef(FileRefList__ *list){
	int length = 0;
	while(list){
		length++;
		list = list->next;
	}
	return length;
}
#define LISTTYPEFileRef
#endif


FileRefList__ * filesystem = NULL;

FileRefList__ * listdir(char * initial){
	FileRefList__ * new_list = malloc(sizeof(FileRefList__ *));
	for(int index=0;index < listLen_FileRef
(filesystem);index++){
		if(initial){
			if(cmpStrHead(get_FileRef
(filesystem, index)->name, initial)){
				push_FileRef
(new_list, get_FileRef
(filesystem, index));
			}
		} else {
			push_FileRef
(new_list, get_FileRef
(filesystem, index));
		}
	}
	return new_list;
}
