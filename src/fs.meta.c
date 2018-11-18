// filesystem of shitOS
// ls - lists all files with names beginning with a string
// read - opens a file and copies its contents to a new bytestream
// create - change a handle

// requires list library
// which requires better macro capabilities

typedef struct File{
	int length;
	char * name;
	char * contents;
} file;

defineList(File)

List(File) filesystem = NULL;

List(File) listdir(char * initial){
	List(File) new_list = malloc(sizeof(List(File)));
	if(initial){
		for(filesystem){
			append(File)(new_list, get(File)(new_list));					
		}
	} else {
		
	}
	return new_list;
}
