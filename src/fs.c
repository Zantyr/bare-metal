// filesystem of shitOS
// ls - lists all files with names beginning with a string
// read - opens a file and copies its contents to a new bytestream
// create - change a handle

// requires list library
// which requires better macro capabilities

typedef struct file{
	int length;
	char * contents;
} file;

