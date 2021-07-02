#define STDLIB

/* in stdLib from other places:
strLen
strRev
strFindCh
cmpStrHead
*/

char* strCopy(char *from){
	char* new = (char*) malloc((int) strLen(from) + 1);
	setStr(new, from);
	return new;
}

void memCopy(char *from, char *to, unsigned int bytes){
	for(unsigned int i=0; i<bytes; i++){
		to[i] = from[i];
	}
}
