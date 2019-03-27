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
	List(FileRef) new_list = NULL;
	for(int index=0;index < listLen(FileRef)(filesystem);index++){
		if(initial){
			if(cmpStrHead(get(FileRef)(filesystem, index)->name, initial)){
				if(new_list){
					append(FileRef)(new_list, get(FileRef)(filesystem, index));
				} else {
					new_list = malloc(sizeof(List(FileRef)));
					new_list->value = get(FileRef)(filesystem, index);
				}
			}
		} else {
			if(new_list){
				append(FileRef)(new_list, get(FileRef)(filesystem, index));
			} else {
				new_list = malloc(sizeof(List(FileRef)));
				new_list->value = get(FileRef)(filesystem, index);
			}
		}
	}
	return new_list;
}

int bash_ls(char *s){
	List(FileRef) flist;
	if(strLen(s)){
		flist = listdir(s);
	} else {
		flist = listdir(NULL);
	}
	if(!flist){
		terminal_cat("No matching files");
		return 1;
	}
	while(flist != NULL){
		terminal_cat(flist->value->name);
		terminal_cat(" - size: ");
		terminal_cat(itoa(flist->value->length));
		terminal_cat("\n");
		flist = flist->next;
	}
	return 0;
}

int add_txt_file(char *name, char *txt){
	if(filesystem){
		short found = 0;
		List(FileRef) search = filesystem;
		while(search){
			if(cmpStrHead(name, search->value->name) && cmpStrHead(search->value->name, name)){
				found = 1;
				search = NULL;
			} else {
				search = search->next;							
			}
		}
		if(!found){
			FileRef new_one = malloc(sizeof(File));
			char * new_name = strCopy(name);
			char * new_txt = strCopy(txt);
			new_one->name = new_name;
			new_one->contents = new_txt;
			new_one->length = strLen(new_txt);
			append(FileRef)(filesystem, new_one);
			return 0;
		} else {
			return 1;
		}
	} else {
		die("NULL filesystem");
	}
	return 1;
}

void init_filesys(){
	FileRef new_one = malloc(sizeof(File));
	new_one->name = "commands";
	new_one->contents = "cat meminfo set printvar color cls memmap malloc_info ls";
	new_one->length = strLen(new_one->contents);
	filesystem = malloc(sizeof(List(FileRef)));
	filesystem->value = new_one;
	filesystem->next = NULL;
	{% files %}
};
